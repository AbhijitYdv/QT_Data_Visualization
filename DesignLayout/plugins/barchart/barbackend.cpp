#include "barbackend.h"

#include <QDateTime>

#include <algorithm>
#include <cmath>

namespace {
QString numberLabel(double x)
{
    if (x == std::floor(x) && std::abs(x) < 1e15)
        return QString::number(static_cast<qint64>(x));
    return QString::number(x, 'g', 4);
}

QString dateLabel(double msecs)
{
    // Same as the old QML: Qt.formatDate(new Date(x), "MM-dd") in local time.
    return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(msecs))
        .toLocalTime()
        .toString(QStringLiteral("MM-dd"));
}
} // namespace

BarBackend::BarBackend(QAbstractItemModel *model, QObject *parent)
    : XYBackend(model, parent)
{
}

void BarBackend::setMaxBars(int bars)
{
    bars = std::max(1, bars);
    if (bars == m_maxBars)
        return;
    m_maxBars = bars;
    emit maxBarsChanged();
    scheduleRefresh();
}

void BarBackend::rebuild()
{
    m_labels.clear();
    m_values.clear();
    m_grouped = false;

    const QList<QPointF> &pts = points();
    auto addBar = [this](double x, double y) {
        m_labels << (xIsDate() ? dateLabel(x) : numberLabel(x));
        m_values << y;
    };

    const int n = static_cast<int>(pts.size());
    if (n <= m_maxBars) {
        for (const QPointF &p : pts)
            addBar(p.x(), p.y());
        return;
    }

    m_grouped = true;
    const double groupSize = static_cast<double>(n) / m_maxBars;
    for (int g = 0; g < m_maxBars; ++g) {
        const int start = static_cast<int>(g * groupSize);
        const int end = std::min(static_cast<int>((g + 1) * groupSize), n);
        if (start >= end)
            continue;
        double sum = 0.0;
        for (int i = start; i < end; ++i)
            sum += pts.at(i).y();
        addBar(pts.at(start).x(), sum / (end - start));
    }
}
