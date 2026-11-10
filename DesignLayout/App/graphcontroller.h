#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include "datasettablemodel.h"
#include "graphtable.h"

#include <QAbstractItemModel>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

class DatasetHistory;

// Loads a data file (CSV, TSV, TXT, JSON, JSON Lines, GeoJSON, SQLite or XML;
// see parsers/dataparser.h) in the background, lets QML choose which
// columns to plot, and feeds the charts.
//
// Large files stay responsive because:
//  - parsing runs on a worker thread (QtConcurrent);
//  - line charts get a thinned copy of the data (min/max per bucket,
//    at most ~2000 points), which looks the same at screen resolution;
//  - the bar chart groups rows into at most 50 averaged bars;
//  - points go into the chart series in one C++ call (fillSeries).
//
// Every successful load is also added to `history` (last 10 datasets this
// session). openFromHistory() brings a dataset back without re-reading the file.
class GraphController : public QObject
{
    Q_OBJECT

    // Data for the charts
    Q_PROPERTY(double minX READ minX NOTIFY dataLoaded)
    Q_PROPERTY(double maxX READ maxX NOTIFY dataLoaded)
    Q_PROPERTY(double minY READ minY NOTIFY dataLoaded)
    Q_PROPERTY(double maxY READ maxY NOTIFY dataLoaded)
    Q_PROPERTY(bool xIsDate READ xIsDate NOTIFY dataLoaded)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY dataLoaded)
    Q_PROPERTY(int plottedCount READ plottedCount NOTIFY dataLoaded)

    // File info
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY tableChanged)
    Q_PROPERTY(int rowCount READ rowCount NOTIFY tableChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)
    Q_PROPERTY(QStringList fileFilters READ fileFilters CONSTANT) // for the FileDialog

    // Choices for the column pickers
    Q_PROPERTY(QStringList xOptions READ xOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList yOptions READ yOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList filterColumnOptions READ filterColumnOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList filterValues READ filterValues NOTIFY selectionChanged)

    // Current selection
    Q_PROPERTY(QString xColumnName READ xColumnName WRITE setXColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString yColumnName READ yColumnName WRITE setYColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString filterColumnName READ filterColumnName WRITE setFilterColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString filterValue READ filterValue WRITE setFilterValue NOTIFY selectionChanged)

    // Summary cards: {count, min, max, mean, median, column} for the Y column
    Q_PROPERTY(QVariantMap summary READ summary NOTIFY dataLoaded)

    // Histogram of the Y column. histogramBins = 0 means "pick automatically".
    Q_PROPERTY(int histogramBins READ histogramBins WRITE setHistogramBins NOTIFY histogramChanged)
    Q_PROPERTY(int histogramAutoBins READ histogramAutoBins NOTIFY dataLoaded)

    // Pie chart: which text column to group by, and whether slices are
    // row counts (false) or sums of the Y column (true).
    Q_PROPERTY(QStringList pieGroupOptions READ pieGroupOptions NOTIFY tableChanged)
    Q_PROPERTY(QString pieGroupColumn READ pieGroupColumn WRITE setPieGroupColumn NOTIFY pieChanged)
    Q_PROPERTY(bool pieSumsY READ pieSumsY WRITE setPieSumsY NOTIFY pieChanged)

    // US map: which number columns hold latitude and longitude (found
    // automatically on load), and how many rows could not be placed.
    Q_PROPERTY(QString mapLatColumn READ mapLatColumn WRITE setMapLatColumn NOTIFY mapChanged)
    Q_PROPERTY(QString mapLonColumn READ mapLonColumn WRITE setMapLonColumn NOTIFY mapChanged)
    Q_PROPERTY(bool mapHasCoordinates READ mapHasCoordinates NOTIFY mapChanged)
    Q_PROPERTY(int mapOutside READ mapOutside NOTIFY mapStatsChanged)
    Q_PROPERTY(int mapMissing READ mapMissing NOTIFY mapStatsChanged)

    // Data table: the filtered rows, sortable by column
    Q_PROPERTY(DatasetTableModel *tableModel READ tableModel CONSTANT)

    // Recent datasets (a list model for the sidebar)
    Q_PROPERTY(QAbstractItemModel *history READ history CONSTANT)
    Q_PROPERTY(int historyCount READ historyCount NOTIFY historyCountChanged)

public:
    enum ColumnType {
        Text = static_cast<int>(CellType::Text),
        Number = static_cast<int>(CellType::Number),
        Date = static_cast<int>(CellType::Date),
    };

    explicit GraphController(QObject *parent = nullptr);

    // Starts loading in the background; returns false if a load is already running.
    Q_INVOKABLE bool loadFile(const QUrl &fileUrl);

    // Re-opens a dataset from the Recent list (row 0 = newest). Replaces the
    // current chart. Returns false while a file is still loading.
    Q_INVOKABLE bool openFromHistory(int row);
    Q_INVOKABLE void removeFromHistory(int row);
    Q_INVOKABLE void clearHistory();

    // Puts the (thinned) points into a QML LineSeries / ScatterSeries in one go.
    Q_INVOKABLE void fillSeries(QObject *series) const;

    // Scatter plots: every point, or an even sample of maxPoints of them.
    Q_INVOKABLE void fillScatterSeries(QObject *series, int maxPoints = 5000) const;

    // Bar chart data: { "x": [...], "y": [...], "grouped": bool }, at most 50 bars.
    Q_INVOKABLE QVariantMap barData() const;

    // Histogram: {labels, counts, edges, maxCount, bins} for the current Y column.
    Q_INVOKABLE QVariantMap histogramData() const;

    // Histogram bars as one outline for a QML AreaSeries's upper LineSeries:
    // each bin is a bar from its lower to upper edge, with a small gap between bars.
    Q_INVOKABLE void fillHistogramSeries(QObject *series) const;

    // Pie: {labels, values, rows, highlight, valueName}. When the pie groups by
    // the filter column, every group is shown and `highlight` is the index of
    // the filtered one (-1 otherwise).
    Q_INVOKABLE QVariantMap pieData() const;

    // Puts the filtered rows' coordinates into a UsMapItem (QML: UsMapItem).
    Q_INVOKABLE void fillMap(QObject *map);

    // A few "column: value" lines describing one row, for tooltips.
    Q_INVOKABLE QString rowSummary(int row) const;

    // All (unthinned) points as QML 'point' values - use only for small data.
    Q_INVOKABLE QVariantList points() const;

    Q_INVOKABLE void clearGraph();

    double minX() const { return m_minX; }
    double maxX() const { return m_maxX; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }
    bool xIsDate() const { return m_xIsDate; }
    int pointCount() const { return static_cast<int>(m_points.size()); }
    int plottedCount() const { return static_cast<int>(m_linePoints.size()); }

    bool loading() const { return m_loading; }
    QString fileName() const { return m_table.fileName; }
    int rowCount() const { return static_cast<int>(m_table.rows.size()); }
    QString statusMessage() const { return m_statusMessage; }
    QStringList fileFilters() const;

    QStringList xOptions() const;
    QStringList yOptions() const;
    QStringList filterColumnOptions() const;
    QStringList filterValues() const { return m_filterValues; }

    QString xColumnName() const;
    QString yColumnName() const;
    QString filterColumnName() const;
    QString filterValue() const { return m_filterValue; }

    void setXColumnName(const QString &name);
    void setYColumnName(const QString &name);
    void setFilterColumnName(const QString &name);
    void setFilterValue(const QString &value);

    QVariantMap summary() const;
    int histogramBins() const { return m_histogramBins; }
    void setHistogramBins(int bins);
    int histogramAutoBins() const;
    QStringList pieGroupOptions() const;
    QString pieGroupColumn() const;
    void setPieGroupColumn(const QString &name);
    bool pieSumsY() const { return m_pieSumsY; }
    void setPieSumsY(bool sums);
    DatasetTableModel *tableModel() const { return m_tableModel; }
    QString mapLatColumn() const { return m_table.headers.value(m_mapLat); }
    QString mapLonColumn() const { return m_table.headers.value(m_mapLon); }
    void setMapLatColumn(const QString &name);
    void setMapLonColumn(const QString &name);
    bool mapHasCoordinates() const { return m_mapLat >= 0 && m_mapLon >= 0; }
    int mapOutside() const { return m_mapOutside; }
    int mapMissing() const { return m_mapMissing; }

    QAbstractItemModel *history() const;
    int historyCount() const;

signals:
    void dataLoaded();
    void graphCleared();
    void statusChanged();
    void tableChanged();
    void selectionChanged();
    void loadingChanged();
    void historyCountChanged();
    void histogramChanged();
    void pieChanged();
    void mapChanged();
    void mapStatsChanged();

private:
    void applyTable(const GraphTable &table, const DatasetSelection *restore = nullptr);
    void pickDefaultColumns();
    void restoreSelection(const DatasetSelection &selection);
    void rememberSelection();
    void refreshFilterValues();
    void rebuildPoints();
    void updateRange();
    void setStatus(const QString &message);
    void setLoading(bool loading);

    GraphTable m_table;

    int m_xCol = -1;      // -1 = use row number
    int m_yCol = -1;      // -1 = nothing to plot
    int m_filterCol = -1; // -1 = no filter
    QString m_filterValue;
    QStringList m_filterValues;

    QVector<int> m_filteredRows;  // row numbers that pass the filter
    QVector<double> m_yValues;    // Y numbers of those rows (summary, histogram)
    int m_histogramBins = 0;      // 0 = automatic
    int m_pieCol = -1;            // text column the pie groups by
    bool m_pieSumsY = false;
    DatasetTableModel *m_tableModel = nullptr; // owned via QObject parent
    int m_mapLat = -1;            // latitude column
    int m_mapLon = -1;            // longitude column
    int m_mapOutside = 0;         // rows outside the lower 48
    int m_mapMissing = 0;         // rows without usable coordinates

    QList<QPointF> m_points;      // every point
    QList<QPointF> m_linePoints;  // thinned for line charts
    QVariantList m_barX;
    QVariantList m_barY;
    bool m_barGrouped = false;

    bool m_xIsDate = false;
    double m_minX = 0.0;
    double m_maxX = 1.0;
    double m_minY = 0.0;
    double m_maxY = 1.0;

    bool m_loading = false;
    QString m_statusMessage;

    DatasetHistory *m_history = nullptr; // owned via QObject parent
    int m_currentEntryId = -1;           // history entry shown right now
};

#endif // GRAPHCONTROLLER_H
