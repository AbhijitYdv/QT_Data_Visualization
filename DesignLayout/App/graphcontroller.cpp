#include "graphcontroller.h"
#include "chartdata.h"
#include "dataparser.h"
#include "datasethistory.h"
#include "parserutils.h"
#include "usmap/usmapdata.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QSet>
#include <QtConcurrent/QtConcurrentRun>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace {

const QString kRowNumber = QStringLiteral("Row #");
const QString kNoFilter = QStringLiteral("(none)");

constexpr int kMaxFilterValues = 500;
constexpr int kTypeSampleRows = 5000;  // rows checked when picking a default filter

using Parsers::parseNumber;

} // namespace

// ===========================================================================

GraphController::GraphController(QObject *parent)
    : QObject(parent)
    , m_tableModel(new DatasetTableModel(this))
    , m_allRowsModel(new DatasetTableModel(this))
    , m_history(new DatasetHistory(this))
{
    connect(m_history, &DatasetHistory::countChanged,
            this, &GraphController::historyCountChanged);
    // Whenever X / Y / filter change, save them on the current history entry.
    connect(this, &GraphController::selectionChanged,
            this, &GraphController::rememberSelection);
}

bool GraphController::loadFile(const QUrl &fileUrl)
{
    if (m_loading)
        return false;

    const QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile()
                                               : fileUrl.toString();
    setLoading(true);
    setStatus(tr("Loading %1…").arg(QFileInfo(path).fileName()));

    auto *watcher = new QFutureWatcher<GraphTable>(this);
    connect(watcher, &QFutureWatcher<GraphTable>::finished, this, [this, watcher]() {
        const GraphTable table = watcher->result();
        watcher->deleteLater();
        setLoading(false);
        if (!table.error.isEmpty()) {
            setStatus(table.error);
            return;
        }

        // Record it in history first; a re-import of the same file keeps
        // the column choices it had last time.
        m_currentEntryId = m_history->add(table);
        m_history->setCurrentId(m_currentEntryId);

        DatasetSelection saved;
        if (const auto *entry = m_history->entryById(m_currentEntryId))
            saved = entry->selection;
        const bool hasSaved = !saved.yColumn.isEmpty();
        applyTable(table, hasSaved ? &saved : nullptr);
    });
    watcher->setFuture(QtConcurrent::run(Parsers::parseDataFile, path));
    return true;
}

bool GraphController::openFromHistory(int row)
{
    if (m_loading)
        return false;
    const DatasetHistory::Entry *entry = m_history->entryAt(row);
    if (!entry)
        return false;
    if (entry->id == m_currentEntryId)
        return true; // already showing

    // Copy before applying: applyTable() writes back into the history.
    const GraphTable table = entry->table;
    const DatasetSelection selection = entry->selection;

    m_currentEntryId = entry->id;
    m_history->setCurrentId(m_currentEntryId);
    applyTable(table, selection.yColumn.isEmpty() ? nullptr : &selection);
    return true;
}

void GraphController::removeFromHistory(int row)
{
    const DatasetHistory::Entry *entry = m_history->entryAt(row);
    if (!entry)
        return;
    // The chart on screen stays; it just isn't linked to an entry any more.
    if (entry->id == m_currentEntryId)
        m_currentEntryId = -1;
    m_history->removeAt(row);
}

void GraphController::clearHistory()
{
    m_currentEntryId = -1;
    m_history->clear();
}

QStringList GraphController::fileFilters() const
{
    return Parsers::fileDialogFilters();
}

QAbstractItemModel *GraphController::history() const
{
    return m_history;
}

int GraphController::historyCount() const
{
    return m_history->count();
}

void GraphController::applyTable(const GraphTable &table, const DatasetSelection *restore)
{
    m_table = table;
    // Row numbers from the previous dataset mean nothing for this one; the
    // cards that refresh on the signals below must not read them.
    m_filteredRows.clear();
    m_yValues.clear();
    pickDefaultColumns();
    m_filterValue.clear();
    if (restore)
        restoreSelection(*restore);
    refreshFilterValues();

    QVector<int> allRows(m_table.rows.size());
    std::iota(allRows.begin(), allRows.end(), 0);
    m_allRowsModel->setSource(m_table, allRows);

    emit tableChanged();
    emit selectionChanged();
    applyFilter();
}

void GraphController::pickDefaultColumns()
{
    // Latitude/longitude go to the map (which finds them itself); the X/Y
    // charts use them only when there is nothing else to plot.
    const UsMap::LatLonColumns coords = UsMap::detectLatLon(m_table);

    QList<int> numberCols, coordCols, dateCols, textCols;
    for (int c = 0; c < m_table.types.size(); ++c) {
        switch (m_table.types.at(c)) {
        case Number: (c == coords.lat || c == coords.lon ? coordCols : numberCols) << c; break;
        case Date: dateCols << c; break;
        default: textCols << c; break;
        }
    }
    const int plainNumbers = static_cast<int>(numberCols.size());
    numberCols << coordCols;

    if (!dateCols.isEmpty())
        m_xCol = dateCols.first();
    else if (plainNumbers >= 2 || (plainNumbers == 0 && numberCols.size() >= 2))
        m_xCol = numberCols.first();
    else
        m_xCol = -1; // plot against the row number

    m_yCol = -1;
    for (int c : std::as_const(numberCols)) {
        if (c != m_xCol) {
            m_yCol = c;
            break;
        }
    }

    // Default filter: first text column with 2-200 distinct values that repeat
    // (e.g. "kind" or "region"). A column where nearly every row is different,
    // such as a name or city, would filter down to a single row, so it is skipped.
    m_filterCol = -1;
    const int sample = std::min<int>(static_cast<int>(m_table.rows.size()), kTypeSampleRows);
    for (int c : std::as_const(textCols)) {
        QSet<QString> distinct;
        for (int r = 0; r < sample && distinct.size() <= 200; ++r)
            distinct.insert(m_table.rows.at(r).value(c).trimmed());
        const bool repeats = distinct.size() * 2 <= sample;
        if (distinct.size() >= 2 && distinct.size() <= 200 && repeats) {
            m_filterCol = c;
            break;
        }
    }
}

// Applies saved column names on top of the defaults. Any name that no
// longer exists (e.g. the file changed) keeps the default instead.
void GraphController::restoreSelection(const DatasetSelection &selection)
{
    if (selection.xColumn == kRowNumber) {
        m_xCol = -1;
    } else {
        const int col = static_cast<int>(m_table.headers.indexOf(selection.xColumn));
        if (col >= 0 && m_table.types.value(col) != Text)
            m_xCol = col;
    }

    const int y = static_cast<int>(m_table.headers.indexOf(selection.yColumn));
    if (y >= 0 && m_table.types.value(y) == Number)
        m_yCol = y;

    if (selection.filterColumn == kNoFilter) {
        m_filterCol = -1;
    } else {
        const int col = static_cast<int>(m_table.headers.indexOf(selection.filterColumn));
        if (col >= 0 && m_table.types.value(col) == Text)
            m_filterCol = col;
    }

    // refreshFilterValues() keeps this if it's still a valid value.
    m_filterValue = selection.filterValue;
}

void GraphController::rememberSelection()
{
    if (m_currentEntryId < 0)
        return;
    m_history->setSelection(m_currentEntryId,
                            {xColumnName(), yColumnName(), filterColumnName(), m_filterValue});
}

void GraphController::refreshFilterValues()
{
    m_filterValues.clear();
    if (m_filterCol < 0) {
        m_filterValue.clear();
        return;
    }

    QSet<QString> seen;
    for (const QStringList &row : std::as_const(m_table.rows)) {
        const QString v = row.value(m_filterCol).trimmed();
        if (!seen.contains(v)) {
            seen.insert(v);
            m_filterValues << v;
            if (m_filterValues.size() >= kMaxFilterValues)
                break;
        }
    }
    if (!m_filterValues.contains(m_filterValue))
        m_filterValue = m_filterValues.value(0);
}

// Rows that pass the filter, shared by every card: the table and the chart
// plugins (through tableModel), and the Y numbers for the summary cards.
void GraphController::applyFilter()
{
    m_filteredRows.clear();
    m_yValues.clear();

    m_filteredRows.reserve(m_table.rows.size());
    for (int r = 0; r < m_table.rows.size(); ++r) {
        if (m_filterCol < 0 || m_table.rows.at(r).value(m_filterCol).trimmed() == m_filterValue)
            m_filteredRows << r;
    }
    m_tableModel->setSource(m_table, m_filteredRows);

    if (m_yCol < 0) {
        setStatus(tr("%1: no number column to plot on Y").arg(m_table.fileName));
        emit dataLoaded();
        return;
    }

    m_yValues.reserve(m_filteredRows.size());
    for (int r : std::as_const(m_filteredRows)) {
        double y = 0.0;
        if (parseNumber(m_table.rows.at(r).value(m_yCol), &y) && std::isfinite(y))
            m_yValues << y;
    }

    QString status = tr("%1 rows · %2 shown").arg(m_table.rows.size()).arg(m_yValues.size());
    if (m_table.skippedLines > 0)
        status += tr(" · %1 lines skipped").arg(m_table.skippedLines);
    setStatus(status);

    emit dataLoaded();
}

// ------------------------------------------------------------- summary

QVariantMap GraphController::summary() const
{
    const ChartData::Summary s = ChartData::summarize(m_yValues);
    return {
        {QStringLiteral("count"), s.count},
        {QStringLiteral("min"), s.min},
        {QStringLiteral("max"), s.max},
        {QStringLiteral("mean"), s.mean},
        {QStringLiteral("median"), s.median},
        {QStringLiteral("column"), yColumnName()},
    };
}

// --------------------------------------------------------------- pickers

QStringList GraphController::xOptions() const
{
    QStringList options{kRowNumber};
    for (int c = 0; c < m_table.types.size(); ++c)
        if (m_table.types.at(c) != Text)
            options << m_table.headers.value(c);
    return options;
}

QStringList GraphController::yOptions() const
{
    QStringList options;
    for (int c = 0; c < m_table.types.size(); ++c)
        if (m_table.types.at(c) == Number)
            options << m_table.headers.value(c);
    return options;
}

QStringList GraphController::filterColumnOptions() const
{
    QStringList options{kNoFilter};
    for (int c = 0; c < m_table.types.size(); ++c)
        if (m_table.types.at(c) == Text)
            options << m_table.headers.value(c);
    return options;
}

QString GraphController::xColumnName() const
{
    return m_xCol < 0 ? kRowNumber : m_table.headers.value(m_xCol);
}

QString GraphController::yColumnName() const
{
    return m_yCol < 0 ? QString() : m_table.headers.value(m_yCol);
}

QString GraphController::filterColumnName() const
{
    return m_filterCol < 0 ? kNoFilter : m_table.headers.value(m_filterCol);
}

void GraphController::setXColumnName(const QString &name)
{
    const int col = (name == kRowNumber) ? -1 : static_cast<int>(m_table.headers.indexOf(name));
    if (col == m_xCol || (col < 0 && name != kRowNumber))
        return;
    m_xCol = col;
    emit selectionChanged();
    applyFilter();
}

void GraphController::setYColumnName(const QString &name)
{
    const int col = static_cast<int>(m_table.headers.indexOf(name));
    if (col < 0 || col == m_yCol)
        return;
    m_yCol = col;
    emit selectionChanged();
    applyFilter();
}

void GraphController::setFilterColumnName(const QString &name)
{
    const int col = (name == kNoFilter || name.isEmpty())
                        ? -1
                        : static_cast<int>(m_table.headers.indexOf(name));
    if (col == m_filterCol)
        return;
    m_filterCol = col;
    m_filterValue.clear();
    refreshFilterValues();
    emit selectionChanged();
    applyFilter();
}

void GraphController::setFilterValue(const QString &value)
{
    if (value == m_filterValue)
        return;
    m_filterValue = value;
    emit selectionChanged();
    applyFilter();
}

// ---------------------------------------------------------------------------

void GraphController::setStatus(const QString &message)
{
    if (m_statusMessage == message)
        return;
    m_statusMessage = message;
    emit statusChanged();
}

void GraphController::setLoading(bool loading)
{
    if (m_loading == loading)
        return;
    m_loading = loading;
    emit loadingChanged();
}
