#include "histogrambackend.h"
#include "chartdata.h"
#include "modelaccess.h"

#include <QMetaObject>
#include <QPointF>
#include <QXYSeries>

#include <algorithm>

HistogramBackend::HistogramBackend(QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    ModelAccess::onAnyChange(m_model, this, [this] { scheduleRefresh(); });
    scheduleRefresh();
}

void HistogramBackend::setYColumn(const QString &name)
{
    if (name == m_yColumn)
        return;
    m_yColumn = name;
    emit yColumnChanged();
    scheduleRefresh();
}

void HistogramBackend::setBins(int bins)
{
    bins = bins <= 0 ? 0 : std::clamp(bins, 1, 200);
    if (bins == m_bins)
        return;
    m_bins = bins;
    emit binsChanged();
    scheduleRefresh();
}

void HistogramBackend::scheduleRefresh()
{
    if (m_queued)
        return;
    m_queued = true;
    QMetaObject::invokeMethod(this, [this] { refresh(); }, Qt::QueuedConnection);
}

QVariantList HistogramBackend::counts() const
{
    QVariantList out;
    for (int c : m_counts)
        out << c;
    return out;
}

QVariantList HistogramBackend::edges() const
{
    QVariantList out;
    for (double e : m_edges)
        out << e;
    return out;
}

void HistogramBackend::refresh()
{
    m_queued = false;

    // A different dataset goes back to automatic bins (like the old card).
    const QStringList headers = ModelAccess::headers(m_model);
    if (headers != m_lastHeaders) {
        const bool firstDataset = m_lastHeaders.isEmpty();
        m_lastHeaders = headers;
        if (!firstDataset && m_bins != 0) {
            m_bins = 0;
            emit binsChanged();
        }
    }

    QVector<double> values;
    const int valueRole = ModelAccess::role(m_model, "value");
    const int yCol = ModelAccess::column(m_model, m_yColumn);
    if (m_model && valueRole >= 0 && yCol >= 0) {
        const int rows = m_model->rowCount();
        values.reserve(rows);
        for (int r = 0; r < rows; ++r) {
            const QVariant v = m_model->index(r, yCol).data(valueRole);
            if (v.isValid())
                values << v.toDouble();
        }
    }

    m_autoBins = ChartData::defaultBinCount(static_cast<int>(values.size()));
    const ChartData::Histogram h =
        ChartData::histogram(values, m_bins > 0 ? m_bins : m_autoBins);
    m_edges = h.edges;
    m_counts = h.counts;
    m_maxCount = h.maxCount;
    m_labels.clear();
    for (int i = 0; i < m_counts.size(); ++i)
        m_labels << ChartData::binLabel(h, i);

    emit changed();
}

void HistogramBackend::fillSeries(QObject *series) const
{
    auto *xy = qobject_cast<QXYSeries *>(series);
    if (!xy)
        return;
    // Each bar: up at its left edge, across, down at its right edge.
    QList<QPointF> outline;
    outline.reserve(m_counts.size() * 4);
    for (int i = 0; i < m_counts.size(); ++i) {
        const double left = m_edges.at(i), right = m_edges.at(i + 1);
        const double gap = (right - left) * 0.06;
        const double count = m_counts.at(i);
        outline << QPointF(left + gap, 0) << QPointF(left + gap, count)
                << QPointF(right - gap, count) << QPointF(right - gap, 0);
    }
    xy->replace(outline);
}
