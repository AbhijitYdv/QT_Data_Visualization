#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include <QList>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

// Result of parsing a file on the background thread.
struct GraphTable
{
    QString fileName;
    QString error;              // non-empty if loading failed
    QStringList headers;
    QList<QStringList> rows;
    QVector<int> types;         // one of the ColumnType values per column
    int skippedLines = 0;
};

// Loads a .csv/.txt table in the background, lets QML choose which
// columns to plot, and feeds the charts.
//
// Large files stay responsive because:
//  - parsing runs on a worker thread (QtConcurrent);
//  - line charts get a thinned copy of the data (min/max per bucket,
//    at most ~2000 points), which looks the same at screen resolution;
//  - the bar chart groups rows into at most 50 averaged bars;
//  - points go into the chart series in one C++ call (fillSeries).
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

public:
    enum ColumnType { Text = 0, Number = 1, Date = 2 };

    explicit GraphController(QObject *parent = nullptr);

    // Starts loading in the background; returns false if a load is already running.
    Q_INVOKABLE bool loadFile(const QUrl &fileUrl);

    // Puts the (thinned) points into a QML LineSeries / ScatterSeries in one go.
    Q_INVOKABLE void fillSeries(QObject *series) const;

    // Bar chart data: { "x": [...], "y": [...], "grouped": bool }, at most 50 bars.
    Q_INVOKABLE QVariantMap barData() const;

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

signals:
    void dataLoaded();
    void graphCleared();
    void statusChanged();
    void tableChanged();
    void selectionChanged();
    void loadingChanged();

private:
    void applyTable(const GraphTable &table);
    void pickDefaultColumns();
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
};

#endif // GRAPHCONTROLLER_H
