#ifndef MAPBACKEND_H
#define MAPBACKEND_H

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>
#include <QPointF>
#include <QStringList>
#include <QVector>

// Data behind UsMap.qml: one point per filtered row at its latitude and
// longitude (lower 48 only). The lat/lon columns are found automatically
// (UsMap::detectLatLon) and can be changed; a new dataset goes back to the
// detected ones.
class MapBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString latColumn READ latColumn WRITE setLatColumn NOTIFY changed)
    Q_PROPERTY(QString lonColumn READ lonColumn WRITE setLonColumn NOTIFY changed)
    Q_PROPERTY(QStringList numberColumns READ numberColumns NOTIFY changed) // picker options
    Q_PROPERTY(bool hasCoordinates READ hasCoordinates NOTIFY changed)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY changed)
    Q_PROPERTY(int outside READ outside NOTIFY changed)   // valid, but outside the lower 48
    Q_PROPERTY(int missing READ missing NOTIFY changed)   // no usable coordinates
    Q_PROPERTY(int rowCount READ rowCount NOTIFY changed)

public:
    explicit MapBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    QString latColumn() const { return m_lat; }
    QString lonColumn() const { return m_lon; }
    void setLatColumn(const QString &name);
    void setLonColumn(const QString &name);
    QStringList numberColumns() const { return m_numberColumns; }
    bool hasCoordinates() const { return !m_lat.isEmpty() && !m_lon.isEmpty(); }
    int pointCount() const { return static_cast<int>(m_lonLat.size()); }
    int outside() const { return m_outside; }
    int missing() const { return m_missing; }
    int rowCount() const { return m_model ? m_model->rowCount() : 0; }

    // `item` must be the plugin's UsMapView; anything else is ignored.
    Q_INVOKABLE void fillMap(QObject *item) const;
    // Up to 5 "column: value" lines for a hovered point (a model row).
    Q_INVOKABLE QString rowSummary(int modelRow) const;
    Q_INVOKABLE void refresh();

signals:
    void changed();

private:
    void scheduleRefresh();
    bool columnInRange(int column, double limit) const;

    QPointer<QAbstractItemModel> m_model;
    QString m_lat;
    QString m_lon;
    QStringList m_lastHeaders;
    bool m_queued = false;

    QStringList m_numberColumns;
    QVector<QPointF> m_lonLat;
    QVector<int> m_rows; // model row of each point
    int m_outside = 0;
    int m_missing = 0;
};

#endif // MAPBACKEND_H
