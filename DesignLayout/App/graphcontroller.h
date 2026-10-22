#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include <QList>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVector>

// Loads a .csv/.txt table, lets QML choose which columns to plot,
// and hands the resulting (x, y) points to every chart.
//
//  - Separator auto-detected: comma, semicolon, tab or spaces.
//  - Header row auto-detected (first row with no numbers in it).
//  - Each column is classified as Number, Date (20221101 / 2022-11-01) or Text.
//  - X can be a number column, a date column, or "Row #".
//  - Y must be a number column.
//  - Optionally keep only rows where a text column equals one value
//    (e.g. kind = だいこん).
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

    // File info
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
    explicit GraphController(QObject *parent = nullptr);

    Q_INVOKABLE bool loadFile(const QUrl &fileUrl);
    Q_INVOKABLE QVariantList points() const;
    Q_INVOKABLE void clearGraph();

    double minX() const { return m_minX; }
    double maxX() const { return m_maxX; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }
    bool xIsDate() const { return m_xIsDate; }
    int pointCount() const { return static_cast<int>(m_points.size()); }

    QString fileName() const { return m_fileName; }
    int rowCount() const { return static_cast<int>(m_rows.size()); }
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

private:
    enum class ColumnType { Text, Number, Date };

    static bool parseNumber(const QString &text, double *value);
    static bool parseDate(const QString &text, double *msecs);
    static QStringList splitLine(const QString &line, QChar delimiter);

    void detectColumnTypes();
    void pickDefaultColumns();
    void refreshFilterValues();
    void rebuildPoints();
    void updateRange();
    void setStatus(const QString &message);

    QStringList m_headers;
    QList<QStringList> m_rows;
    QVector<ColumnType> m_types;

    int m_xCol = -1;      // -1 = use row number
    int m_yCol = -1;      // -1 = nothing to plot
    int m_filterCol = -1; // -1 = no filter
    QString m_filterValue;
    QStringList m_filterValues;

    QList<QPointF> m_points;
    bool m_xIsDate = false;
    double m_minX = 0.0;
    double m_maxX = 1.0;
    double m_minY = 0.0;
    double m_maxY = 1.0;

    QString m_fileName;
    QString m_statusMessage;
    int m_skippedLines = 0;
};

#endif // GRAPHCONTROLLER_H
