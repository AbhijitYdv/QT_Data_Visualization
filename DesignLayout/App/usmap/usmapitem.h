#ifndef USMAPITEM_H
#define USMAPITEM_H

#include <QHash>
#include <QImage>
#include <QPointF>
#include <QQuickPaintedItem>
#include <QTransform>
#include <QVector>

// Draws the lower 48 states with one dot per data point, using QPainter.
//
// Fill it from QML with graphController.fillMap(map). The state outlines are
// drawn once into a cached image and only redrawn when the item is resized;
// the dots are drawn on top. Hovering finds the nearest dot within 6 px
// (through a grid, so it stays fast with many points), or else the state
// under the cursor.
class UsMapItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int pointCount READ pointCount NOTIFY pointsChanged)
    Q_PROPERTY(int hoveredRow READ hoveredRow NOTIFY hoverChanged)       // table row, or -1
    Q_PROPERTY(QString hoveredState READ hoveredState NOTIFY hoverChanged)
    Q_PROPERTY(QPointF hoverPosition READ hoverPosition NOTIFY hoverChanged)

public:
    explicit UsMapItem(QQuickItem *parent = nullptr);

    // lonLat: x = longitude, y = latitude; rows: the table row of each point.
    void setPoints(const QVector<QPointF> &lonLat, const QVector<int> &rows);

    int pointCount() const { return static_cast<int>(m_rows.size()); }
    int hoveredRow() const { return m_hoverIndex < 0 ? -1 : m_rows.at(m_hoverIndex); }
    QString hoveredState() const { return m_hoverState; }
    QPointF hoverPosition() const { return m_hoverPos; }

    void paint(QPainter *painter) override;

signals:
    void pointsChanged();
    void hoverChanged();

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    void updateLayout();          // transform, screen positions, grid, cache
    int nearestPoint(const QPointF &pos) const;
    static quint64 cellKey(int cx, int cy);

    QVector<QPointF> m_projected; // map units (y north)
    QVector<QPointF> m_screen;    // item pixels
    QVector<int> m_rows;
    QTransform m_toScreen;        // map units -> item pixels
    QHash<quint64, QVector<int>> m_grid; // cell -> point indexes
    QImage m_statesImage;         // cached outlines at the current size

    int m_hoverIndex = -1;
    int m_hoverStateIndex = -1;
    QString m_hoverState;
    QPointF m_hoverPos;
};

#endif // USMAPITEM_H
