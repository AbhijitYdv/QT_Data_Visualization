#include "graphcontroller.h"
#include "dataparser.h"
#include "datasethistory.h"
#include "parserutils.h"

#include <QFileInfo>
#include <QFutureWatcher>
#include <QSet>
#include <QXYSeries>
#include <QtConcurrent/QtConcurrentRun>

#include <algorithm>
#include <limits>

namespace {

const QString kRowNumber = QStringLiteral("Row #");
const QString kNoFilter = QStringLiteral("(none)");

constexpr int kMaxFilterValues = 500;
constexpr int kTypeSampleRows = 5000;  // rows checked when picking a default filter
constexpr int kMaxLinePoints = 2000;   // per line chart after thinning
constexpr int kMaxBars = 50;           // bars in the bar chart

using Parsers::parseDate;
using Parsers::parseNumber;

// Keeps the lowest and highest point of each bucket, in order, so peaks
// and dips survive while the point count drops to about maxPoints.
QList<QPointF> thinForLine(const QList<QPointF> &pts, int maxPoints)
{
    if (pts.size() <= maxPoints)
        return pts;

    const int buckets = maxPoints / 2;
    const double bucketSize = static_cast<double>(pts.size()) / buckets;
    QList<QPointF> out;
    out.reserve(maxPoints + 2);

    for (int b = 0; b < buckets; ++b) {
        const int start = static_cast<int>(b * bucketSize);
        const int end = std::min(static_cast<int>((b + 1) * bucketSize),
                                 static_cast<int>(pts.size()));
        if (start >= end)
            continue;
        int lo = start, hi = start;
        for (int i = start + 1; i < end; ++i) {
            if (pts.at(i).y() < pts.at(lo).y()) lo = i;
            if (pts.at(i).y() > pts.at(hi).y()) hi = i;
        }
        if (lo == hi)
            out << pts.at(lo);
        else if (lo < hi)
            out << pts.at(lo) << pts.at(hi);
        else
            out << pts.at(hi) << pts.at(lo);
    }
    return out;
}

} // namespace

// ===========================================================================

GraphController::GraphController(QObject *parent)
    : QObject(parent)
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
    pickDefaultColumns();
    m_filterValue.clear();
    if (restore)
        restoreSelection(*restore);
    refreshFilterValues();

    emit tableChanged();
    emit selectionChanged();
    rebuildPoints();
}

void GraphController::pickDefaultColumns()
{
    QList<int> numberCols, dateCols, textCols;
    for (int c = 0; c < m_table.types.size(); ++c) {
        switch (m_table.types.at(c)) {
        case Number: numberCols << c; break;
        case Date: dateCols << c; break;
        default: textCols << c; break;
        }
    }

    if (!dateCols.isEmpty())
        m_xCol = dateCols.first();
    else if (numberCols.size() >= 2)
        m_xCol = numberCols.first();
    else
        m_xCol = -1;

    m_yCol = -1;
    for (int c : std::as_const(numberCols)) {
        if (c != m_xCol) {
            m_yCol = c;
            break;
        }
    }

    // Default filter: first text column with 2-200 distinct values (e.g. "kind").
    m_filterCol = -1;
    const int sample = std::min<int>(static_cast<int>(m_table.rows.size()), kTypeSampleRows);
    for (int c : std::as_const(textCols)) {
        QSet<QString> distinct;
        for (int r = 0; r < sample && distinct.size() <= 200; ++r)
            distinct.insert(m_table.rows.at(r).value(c).trimmed());
        if (distinct.size() >= 2 && distinct.size() <= 200) {
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

void GraphController::rebuildPoints()
{
    m_points.clear();
    m_linePoints.clear();
    m_barX.clear();
    m_barY.clear();
    m_barGrouped = false;
    m_xIsDate = m_xCol >= 0 && m_table.types.value(m_xCol) == Date;

    if (m_yCol < 0) {
        updateRange();
        setStatus(tr("%1: no number column to plot on Y").arg(m_table.fileName));
        emit dataLoaded();
        return;
    }

    m_points.reserve(m_table.rows.size());
    int shownRows = 0;
    for (const QStringList &row : std::as_const(m_table.rows)) {
        if (m_filterCol >= 0 && row.value(m_filterCol).trimmed() != m_filterValue)
            continue;
        ++shownRows;

        double y = 0.0;
        if (!parseNumber(row.value(m_yCol), &y))
            continue;

        double x = 0.0;
        if (m_xCol < 0)
            x = shownRows;
        else if (m_xIsDate ? !parseDate(row.value(m_xCol), &x)
                           : !parseNumber(row.value(m_xCol), &x))
            continue;

        m_points.append(QPointF(x, y));
    }

    if (m_xCol >= 0) {
        std::stable_sort(m_points.begin(), m_points.end(),
                         [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    }

    // Thinned copy for the line charts.
    m_linePoints = thinForLine(m_points, kMaxLinePoints);

    // Bar chart: one bar per point, or averaged groups of points.
    const int n = static_cast<int>(m_points.size());
    if (n <= kMaxBars) {
        for (const QPointF &p : std::as_const(m_points)) {
            m_barX << p.x();
            m_barY << p.y();
        }
    } else {
        m_barGrouped = true;
        const double groupSize = static_cast<double>(n) / kMaxBars;
        for (int g = 0; g < kMaxBars; ++g) {
            const int start = static_cast<int>(g * groupSize);
            const int end = std::min(static_cast<int>((g + 1) * groupSize), n);
            if (start >= end)
                continue;
            double sum = 0.0;
            for (int i = start; i < end; ++i)
                sum += m_points.at(i).y();
            m_barX << m_points.at(start).x();
            m_barY << sum / (end - start);
        }
    }

    updateRange();

    QString status = tr("%1 rows · %2 shown").arg(m_table.rows.size()).arg(m_points.size());
    if (m_linePoints.size() < m_points.size())
        status += tr(" (drawn as %1)").arg(m_linePoints.size());
    if (m_table.skippedLines > 0)
        status += tr(" · %1 lines skipped").arg(m_table.skippedLines);
    setStatus(status);

    emit dataLoaded();
}

void GraphController::fillSeries(QObject *series) const
{
    if (auto *xy = qobject_cast<QXYSeries *>(series))
        xy->replace(m_linePoints); // one call, one repaint
}

QVariantMap GraphController::barData() const
{
    return {
        {QStringLiteral("x"), m_barX},
        {QStringLiteral("y"), m_barY},
        {QStringLiteral("grouped"), m_barGrouped},
    };
}

QVariantList GraphController::points() const
{
    QVariantList list;
    list.reserve(m_points.size());
    for (const QPointF &p : m_points)
        list.append(p);
    return list;
}

void GraphController::clearGraph()
{
    // Clearing the chart doesn't touch the history; the entry just stops
    // being highlighted as "current".
    m_currentEntryId = -1;
    m_history->setCurrentId(-1);

    m_table = GraphTable();
    m_xCol = m_yCol = m_filterCol = -1;
    m_filterValue.clear();
    m_filterValues.clear();
    m_points.clear();
    m_linePoints.clear();
    m_barX.clear();
    m_barY.clear();
    updateRange();

    emit tableChanged();
    emit selectionChanged();
    emit graphCleared();
    emit dataLoaded();
    setStatus(QString());
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
    rebuildPoints();
}

void GraphController::setYColumnName(const QString &name)
{
    const int col = static_cast<int>(m_table.headers.indexOf(name));
    if (col < 0 || col == m_yCol)
        return;
    m_yCol = col;
    emit selectionChanged();
    rebuildPoints();
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
    rebuildPoints();
}

void GraphController::setFilterValue(const QString &value)
{
    if (value == m_filterValue)
        return;
    m_filterValue = value;
    emit selectionChanged();
    rebuildPoints();
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

void GraphController::updateRange()
{
    if (m_points.isEmpty()) {
        m_minX = 0.0; m_maxX = 1.0;
        m_minY = 0.0; m_maxY = 1.0;
        return;
    }

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    for (const QPointF &p : std::as_const(m_points)) {
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }

    double minX = m_points.first().x();
    double maxX = m_points.last().x();
    if (m_xCol < 0 || minX > maxX) { // not sorted - scan
        minX = std::numeric_limits<double>::max();
        maxX = std::numeric_limits<double>::lowest();
        for (const QPointF &p : std::as_const(m_points)) {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
        }
    }

    const double spanX = maxX - minX;
    const double spanY = maxY - minY;
    const double padX = spanX > 0 ? spanX * 0.05 : (m_xIsDate ? 86400000.0 : 1.0);
    const double padY = spanY > 0 ? spanY * 0.05 : 1.0;

    m_minX = minX - padX; m_maxX = maxX + padX;
    m_minY = minY - padY; m_maxY = maxY + padY;
}
