#ifndef XYBACKEND_H
#define XYBACKEND_H

#include <QAbstractItemModel>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QPointer>

// Shared base for every chart that plots one X column against one Y column
// (bar, line, area, scatter). Reads the filtered dataset model through roles
// looked up by name ("value", "sourceRow", "columnType"), so plugins never
// include app headers.
//
// points(): every row whose Y (and X) parses, as (x, y)
//  - X = the X column, sorted ascending; dates are ms since epoch;
//  - X = row number (file order, 1-based) when xColumn isn't a column, e.g. "Row #".
// minX..maxY are padded 5% for axes (same as GraphController::updateRange);
// dataMinY / dataMaxY are the real extremes.
//
// Rebuilds once per event-loop turn after the model or the columns change,
// then emits changed(). Subclasses add their own output in rebuild().
class XYBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString xColumn READ xColumn WRITE setXColumn NOTIFY xColumnChanged)
    Q_PROPERTY(QString yColumn READ yColumn WRITE setYColumn NOTIFY yColumnChanged)

    Q_PROPERTY(bool xIsDate READ xIsDate NOTIFY changed)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY changed)
    Q_PROPERTY(double minX READ minX NOTIFY changed)
    Q_PROPERTY(double maxX READ maxX NOTIFY changed)
    Q_PROPERTY(double minY READ minY NOTIFY changed)
    Q_PROPERTY(double maxY READ maxY NOTIFY changed)
    Q_PROPERTY(double dataMinY READ dataMinY NOTIFY changed)
    Q_PROPERTY(double dataMaxY READ dataMaxY NOTIFY changed)

public:
    explicit XYBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    QString xColumn() const { return m_xColumn; }
    QString yColumn() const { return m_yColumn; }
    void setXColumn(const QString &name);
    void setYColumn(const QString &name);

    bool xIsDate() const { return m_xIsDate; }
    int pointCount() const { return static_cast<int>(m_points.size()); }
    double minX() const { return m_minX; }
    double maxX() const { return m_maxX; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }
    double dataMinY() const { return m_dataMinY; }
    double dataMaxY() const { return m_dataMaxY; }
    const QList<QPointF> &points() const { return m_points; }

    // Rebuilds now (normally automatic).
    Q_INVOKABLE void refresh();

signals:
    void xColumnChanged();
    void yColumnChanged();
    void changed();

protected:
    // Called by refresh() after points() is rebuilt, before changed().
    virtual void rebuild() {}
    void scheduleRefresh();

private:
    void buildPoints();
    void updateRange();
    int columnIndex(const QString &name) const;
    int role(const char *name) const;

    QPointer<QAbstractItemModel> m_model;
    QString m_xColumn;
    QString m_yColumn;
    bool m_refreshQueued = false;

    QList<QPointF> m_points;
    bool m_xIsDate = false;
    bool m_sortedByX = false;
    double m_minX = 0.0, m_maxX = 1.0, m_minY = 0.0, m_maxY = 1.0;
    double m_dataMinY = 0.0, m_dataMaxY = 1.0;
};

#endif // XYBACKEND_H
