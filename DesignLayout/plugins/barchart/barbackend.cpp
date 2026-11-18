#include "barbackend.h"

#include <QDateTime>
#include <QMetaObject>
#include <QPointF>
#include <QTimeZone>
#include <QVector>

#include <algorithm>
#include <cmath>

namespace {
constexpr int kDateType = 2; // CellType::Date in graphtable.h

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
    : QObject(parent)
    , m_model(model)
{
    if (m_model) {
        // Every way the model can change ends in one refresh per event loop
        // turn, however many signals arrive.
        connect(m_model, &QAbstractItemModel::modelReset, this, &BarBackend::scheduleRefresh);
        connect(m_model, &QAbstractItemModel::dataChanged, this, &BarBackend::scheduleRefresh);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &BarBackend::scheduleRefresh);
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, &BarBackend::scheduleRefresh);
        connect(m_model, &QAbstractItemModel::layoutChanged, this, &BarBackend::scheduleRefresh);
        connect(m_model, &QAbstractItemModel::headerDataChanged, this, &BarBackend::scheduleRefresh);
    }
    refresh();
}

void BarBackend::setXColumn(const QString &name)
{
    if (name == m_xColumn)
        return;
    m_xColumn = name;
    emit xColumnChanged();
    scheduleRefresh();
}

void BarBackend::setYColumn(const QString &name)
{
    if (name == m_yColumn)
        return;
    m_yColumn = name;
    emit yColumnChanged();
    scheduleRefresh();
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

void BarBackend::scheduleRefresh()
{
    if (m_refreshQueued)
        return;
    m_refreshQueued = true;
    QMetaObject::invokeMethod(this, &BarBackend::refresh, Qt::QueuedConnection);
}

int BarBackend::columnIndex(const QString &name) const
{
    if (!m_model || name.isEmpty())
        return -1;
    for (int c = 0; c < m_model->columnCount(); ++c) {
        if (m_model->headerData(c, Qt::Horizontal).toString() == name)
            return c;
    }
    return -1;
}

int BarBackend::role(const char *name) const
{
    if (!m_model)
        return -1;
    const QHash<int, QByteArray> names = m_model->roleNames();
    for (auto it = names.begin(); it != names.end(); ++it) {
        if (it.value() == name)
            return it.key();
    }
    return -1;
}

void BarBackend::refresh()
{
    m_refreshQueued = false;
    m_labels.clear();
    m_values.clear();
    m_grouped = false;
    m_xIsDate = false;
    m_pointCount = 0;
    m_minY = 0.0;
    m_maxY = 1.0;

    const int valueRole = role("value");
    const int sourceRowRole = role("sourceRow");
    const int typeRole = role("columnType");
    const int yCol = columnIndex(m_yColumn);
    const int xCol = columnIndex(m_xColumn); // -1 = row number

    if (!m_model || valueRole < 0 || sourceRowRole < 0 || yCol < 0) {
        emit changed();
        return;
    }
    if (xCol >= 0 && typeRole >= 0)
        m_xIsDate = m_model->headerData(xCol, Qt::Horizontal, typeRole).toInt() == kDateType;

    // Visit rows in file order, so "Row #" doesn't change when the table is sorted.
    const int rows = m_model->rowCount();
    QVector<QPair<int, int>> order; // (file row, model row)
    order.reserve(rows);
    for (int r = 0; r < rows; ++r)
        order.append({m_model->index(r, 0).data(sourceRowRole).toInt(), r});
    std::sort(order.begin(), order.end());

    QList<QPointF> points;
    points.reserve(rows);
    int shownRows = 0;
    for (const auto &[fileRow, r] : std::as_const(order)) {
        Q_UNUSED(fileRow)
        ++shownRows; // counted even when Y is missing (matches the old chart)
        const QVariant yv = m_model->index(r, yCol).data(valueRole);
        if (!yv.isValid())
            continue;
        double x = shownRows;
        if (xCol >= 0) {
            const QVariant xv = m_model->index(r, xCol).data(valueRole);
            if (!xv.isValid())
                continue;
            x = xv.toDouble();
        }
        points.append(QPointF(x, yv.toDouble()));
    }

    if (xCol >= 0) {
        std::stable_sort(points.begin(), points.end(),
                         [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    }

    m_pointCount = static_cast<int>(points.size());
    if (!points.isEmpty()) {
        const auto [lo, hi] = std::minmax_element(
            points.begin(), points.end(),
            [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
        m_minY = lo->y();
        m_maxY = hi->y();
    }

    auto addBar = [this](double x, double y) {
        m_labels << (m_xIsDate ? dateLabel(x) : numberLabel(x));
        m_values << y;
    };

    const int n = m_pointCount;
    if (n <= m_maxBars) {
        for (const QPointF &p : std::as_const(points))
            addBar(p.x(), p.y());
    } else {
        m_grouped = true;
        const double groupSize = static_cast<double>(n) / m_maxBars;
        for (int g = 0; g < m_maxBars; ++g) {
            const int start = static_cast<int>(g * groupSize);
            const int end = std::min(static_cast<int>((g + 1) * groupSize), n);
            if (start >= end)
                continue;
            double sum = 0.0;
            for (int i = start; i < end; ++i)
                sum += points.at(i).y();
            addBar(points.at(start).x(), sum / (end - start));
        }
    }

    emit changed();
}
