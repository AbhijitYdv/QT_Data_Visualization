#include "graphcontroller.h"
#include "chartdata.h"
#include "dataparser.h"
#include "datasethistory.h"
#include "parserutils.h"
#include "usmap/usmapdata.h"
#include "usmap/usmapitem.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QSet>
#include <QXYSeries>
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
    , m_tableModel(new DatasetTableModel(this))
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

    // The pie starts on the filter column, or else the first text column.
    m_pieCol = m_filterCol;
    for (int c = 0; m_pieCol < 0 && c < m_table.types.size(); ++c)
        if (m_table.types.at(c) == Text)
            m_pieCol = c;
    m_histogramBins = 0;

    emit tableChanged();
    emit selectionChanged();
    emit pieChanged();
    emit mapChanged();
    emit histogramChanged();
    rebuildPoints();
}

void GraphController::pickDefaultColumns()
{
    // Latitude/longitude go to the map; the X/Y charts use them only when
    // there is nothing else to plot.
    const UsMap::LatLonColumns coords = UsMap::detectLatLon(m_table);
    m_mapLat = coords.lat;
    m_mapLon = coords.lon;

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

void GraphController::rebuildPoints()
{
    m_points.clear();
    m_linePoints.clear();
    m_barX.clear();
    m_barY.clear();
    m_barGrouped = false;
    m_xIsDate = m_xCol >= 0 && m_table.types.value(m_xCol) == Date;

    m_filteredRows.clear();
    m_yValues.clear();

    // Rows that pass the filter: shared by every card, the table included.
    m_filteredRows.reserve(m_table.rows.size());
    for (int r = 0; r < m_table.rows.size(); ++r) {
        if (m_filterCol < 0 || m_table.rows.at(r).value(m_filterCol).trimmed() == m_filterValue)
            m_filteredRows << r;
    }
    m_tableModel->setSource(m_table, m_filteredRows);

    if (m_yCol < 0) {
        updateRange();
        setStatus(tr("%1: no number column to plot on Y").arg(m_table.fileName));
        emit dataLoaded();
        return;
    }

    m_points.reserve(m_filteredRows.size());
    m_yValues.reserve(m_filteredRows.size());
    int shownRows = 0;
    for (int r : std::as_const(m_filteredRows)) {
        const QStringList &row = m_table.rows.at(r);
        ++shownRows;

        double y = 0.0;
        if (!parseNumber(row.value(m_yCol), &y) || !std::isfinite(y))
            continue;
        m_yValues << y; // counted even when X is missing

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
    m_filteredRows.clear();
    m_yValues.clear();
    m_pieCol = -1;
    m_mapLat = m_mapLon = -1;
    m_tableModel->clear();
    updateRange();

    emit tableChanged();
    emit pieChanged();
    emit mapChanged();
    emit selectionChanged();
    emit graphCleared();
    emit dataLoaded();
    setStatus(QString());
}

// ------------------------------------------------- summary, histogram, pie

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

int GraphController::histogramAutoBins() const
{
    return ChartData::defaultBinCount(static_cast<int>(m_yValues.size()));
}

void GraphController::setHistogramBins(int bins)
{
    bins = bins <= 0 ? 0 : std::clamp(bins, 1, 200);
    if (bins == m_histogramBins)
        return;
    m_histogramBins = bins;
    emit histogramChanged();
}

QVariantMap GraphController::histogramData() const
{
    const int bins = m_histogramBins > 0 ? m_histogramBins : histogramAutoBins();
    const ChartData::Histogram h = ChartData::histogram(m_yValues, bins);

    QVariantList labels, counts, edges;
    for (int i = 0; i < h.counts.size(); ++i) {
        labels << ChartData::binLabel(h, i);
        counts << h.counts.at(i);
    }
    for (double e : h.edges)
        edges << e;
    return {
        {QStringLiteral("labels"), labels},
        {QStringLiteral("counts"), counts},
        {QStringLiteral("edges"), edges},
        {QStringLiteral("maxCount"), h.maxCount},
        {QStringLiteral("bins"), static_cast<int>(h.counts.size())},
    };
}

void GraphController::fillHistogramSeries(QObject *series) const
{
    auto *xy = qobject_cast<QXYSeries *>(series);
    if (!xy)
        return;
    const int bins = m_histogramBins > 0 ? m_histogramBins : histogramAutoBins();
    const ChartData::Histogram h = ChartData::histogram(m_yValues, bins);

    // Each bar: up at its left edge, across, down at its right edge.
    QList<QPointF> outline;
    outline.reserve(h.counts.size() * 4);
    for (int i = 0; i < h.counts.size(); ++i) {
        const double left = h.edges.at(i), right = h.edges.at(i + 1);
        const double gap = (right - left) * 0.06;
        const double count = h.counts.at(i);
        outline << QPointF(left + gap, 0) << QPointF(left + gap, count)
                << QPointF(right - gap, count) << QPointF(right - gap, 0);
    }
    xy->replace(outline);
}

QStringList GraphController::pieGroupOptions() const
{
    QStringList options;
    for (int c = 0; c < m_table.types.size(); ++c)
        if (m_table.types.at(c) == Text)
            options << m_table.headers.value(c);
    return options;
}

QString GraphController::pieGroupColumn() const
{
    return m_pieCol < 0 ? QString() : m_table.headers.value(m_pieCol);
}

void GraphController::setPieGroupColumn(const QString &name)
{
    const int col = static_cast<int>(m_table.headers.indexOf(name));
    if (col < 0 || col == m_pieCol || m_table.types.value(col) != Text)
        return;
    m_pieCol = col;
    emit pieChanged();
}

void GraphController::setPieSumsY(bool sums)
{
    if (sums == m_pieSumsY)
        return;
    m_pieSumsY = sums;
    emit pieChanged();
}

QVariantMap GraphController::pieData() const
{
    QVariantList labels, values, rows;
    int highlight = -1;
    const bool sums = m_pieSumsY && m_yCol >= 0;

    if (m_pieCol >= 0) {
        // Grouping by the filter column: show every group, mark the filtered one.
        const bool byFilter = m_pieCol == m_filterCol;
        QVector<int> source = m_filteredRows;
        if (byFilter) {
            source.resize(m_table.rows.size());
            std::iota(source.begin(), source.end(), 0);
        }

        const auto groups = ChartData::groups(m_table, source, m_pieCol, sums ? m_yCol : -1);
        for (const ChartData::Group &g : groups) {
            const QString filterLabel = m_filterValue.isEmpty()
                                            ? QCoreApplication::translate("ChartData", "(blank)")
                                            : m_filterValue;
            if (byFilter && !g.isOther && g.label == filterLabel)
                highlight = static_cast<int>(labels.size());
            labels << g.label;
            values << g.value;
            rows << g.rows;
        }
    }

    return {
        {QStringLiteral("labels"), labels},
        {QStringLiteral("values"), values},
        {QStringLiteral("rows"), rows},
        {QStringLiteral("highlight"), highlight},
        {QStringLiteral("valueName"), sums ? tr("Sum of %1").arg(yColumnName()) : tr("Rows")},
    };
}

// ------------------------------------------------------------------ map

void GraphController::setMapLatColumn(const QString &name)
{
    const int col = static_cast<int>(m_table.headers.indexOf(name));
    if (col < 0 || col == m_mapLat || m_table.types.value(col) != Number)
        return;
    m_mapLat = col;
    emit mapChanged();
}

void GraphController::setMapLonColumn(const QString &name)
{
    const int col = static_cast<int>(m_table.headers.indexOf(name));
    if (col < 0 || col == m_mapLon || m_table.types.value(col) != Number)
        return;
    m_mapLon = col;
    emit mapChanged();
}

void GraphController::fillMap(QObject *map)
{
    auto *item = qobject_cast<UsMapItem *>(map);
    if (!item)
        return;
    const UsMap::Points points = UsMap::extractPoints(m_table, m_filteredRows, {m_mapLat, m_mapLon});
    item->setPoints(points.lonLat, points.rows);
    if (points.outside != m_mapOutside || points.missing != m_mapMissing) {
        m_mapOutside = points.outside;
        m_mapMissing = points.missing;
        emit mapStatsChanged();
    }
}

QString GraphController::rowSummary(int row) const
{
    if (row < 0 || row >= m_table.rows.size())
        return {};
    QStringList lines;
    const QStringList &cells = m_table.rows.at(row);
    for (int c = 0; c < m_table.headers.size() && lines.size() < 5; ++c) {
        if (c == m_mapLat || c == m_mapLon || cells.value(c).trimmed().isEmpty())
            continue;
        lines << QStringLiteral("%1: %2").arg(m_table.headers.at(c), cells.value(c));
    }
    return lines.join(u'\n');
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
