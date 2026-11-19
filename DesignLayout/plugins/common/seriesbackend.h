#ifndef SERIESBACKEND_H
#define SERIESBACKEND_H

#include "xybackend.h"

// Backend for charts drawn as a QtCharts XY series (line, area, scatter).
//
// mode Thin   (line, area):  more than maxPoints points -> min and max of
//                            each bucket, which looks the same on screen;
// mode Sample (scatter):     more than maxPoints points -> every n-th point,
//                            which keeps the shape of the cloud.
//
// QML: call backend.fillSeries(series) when `changed` fires; it replaces the
// series' points in one call (one repaint).
class SeriesBackend : public XYBackend
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int maxPoints READ maxPoints WRITE setMaxPoints NOTIFY maxPointsChanged)
    Q_PROPERTY(int plottedCount READ plottedCount NOTIFY changed)

public:
    enum Mode { Thin, Sample };
    Q_ENUM(Mode)

    explicit SeriesBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    Mode mode() const { return m_mode; }
    void setMode(Mode mode);
    int maxPoints() const { return m_maxPoints; }
    void setMaxPoints(int points);
    int plottedCount() const { return static_cast<int>(m_plotted.size()); }
    const QList<QPointF> &plotted() const { return m_plotted; }

    // `series` must be a QXYSeries (LineSeries, ScatterSeries, an AreaSeries'
    // upperSeries). Anything else is ignored.
    Q_INVOKABLE void fillSeries(QObject *series) const;

    static QList<QPointF> thin(const QList<QPointF> &points, int maxPoints);
    static QList<QPointF> sample(const QList<QPointF> &points, int maxPoints);

signals:
    void modeChanged();
    void maxPointsChanged();

protected:
    void rebuild() override;

private:
    Mode m_mode = Thin;
    int m_maxPoints = 2000;
    QList<QPointF> m_plotted;
};

#endif // SERIESBACKEND_H
