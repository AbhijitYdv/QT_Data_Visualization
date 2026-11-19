#include "seriesbackend.h"

#include <QXYSeries>

#include <algorithm>

SeriesBackend::SeriesBackend(QAbstractItemModel *model, QObject *parent)
    : XYBackend(model, parent)
{
}

void SeriesBackend::setMode(Mode mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged();
    scheduleRefresh();
}

void SeriesBackend::setMaxPoints(int points)
{
    points = std::max(2, points);
    if (points == m_maxPoints)
        return;
    m_maxPoints = points;
    emit maxPointsChanged();
    scheduleRefresh();
}

void SeriesBackend::rebuild()
{
    m_plotted = m_mode == Thin ? thin(points(), m_maxPoints) : sample(points(), m_maxPoints);
}

void SeriesBackend::fillSeries(QObject *series) const
{
    if (auto *xy = qobject_cast<QXYSeries *>(series))
        xy->replace(m_plotted);
}

// Same as GraphController's thinForLine(): min and max of each bucket, in
// X order, so peaks and dips survive.
QList<QPointF> SeriesBackend::thin(const QList<QPointF> &pts, int maxPoints)
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

// Same as GraphController::fillScatterSeries(): an even sample.
QList<QPointF> SeriesBackend::sample(const QList<QPointF> &pts, int maxPoints)
{
    if (maxPoints <= 0 || pts.size() <= maxPoints)
        return pts;
    QList<QPointF> out;
    out.reserve(maxPoints);
    const double step = static_cast<double>(pts.size()) / maxPoints;
    for (int i = 0; i < maxPoints; ++i)
        out << pts.at(static_cast<qsizetype>(i * step));
    return out;
}
