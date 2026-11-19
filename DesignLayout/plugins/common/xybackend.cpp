#include "xybackend.h"

#include <QMetaObject>
#include <QVector>

#include <algorithm>
#include <limits>

namespace {
constexpr int kDateType = 2; // CellType::Date in graphtable.h
}

XYBackend::XYBackend(QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    if (m_model) {
        // Every way the model can change ends in one refresh per event-loop
        // turn. Lambdas (not &XYBackend::...) so Qt never has to check this
        // object's class across library boundaries.
        auto later = [this] { scheduleRefresh(); };
        connect(m_model, &QAbstractItemModel::modelReset, this, later);
        connect(m_model, &QAbstractItemModel::dataChanged, this, later);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, later);
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, later);
        connect(m_model, &QAbstractItemModel::layoutChanged, this, later);
        connect(m_model, &QAbstractItemModel::headerDataChanged, this, later);
    }
    // Subclasses aren't constructed yet, so the first build is queued too.
    scheduleRefresh();
}

void XYBackend::setXColumn(const QString &name)
{
    if (name == m_xColumn)
        return;
    m_xColumn = name;
    emit xColumnChanged();
    scheduleRefresh();
}

void XYBackend::setYColumn(const QString &name)
{
    if (name == m_yColumn)
        return;
    m_yColumn = name;
    emit yColumnChanged();
    scheduleRefresh();
}

void XYBackend::scheduleRefresh()
{
    if (m_refreshQueued)
        return;
    m_refreshQueued = true;
    QMetaObject::invokeMethod(this, [this] { refresh(); }, Qt::QueuedConnection);
}

void XYBackend::refresh()
{
    m_refreshQueued = false;
    buildPoints();
    updateRange();
    rebuild();
    emit changed();
}

int XYBackend::columnIndex(const QString &name) const
{
    if (!m_model || name.isEmpty())
        return -1;
    for (int c = 0; c < m_model->columnCount(); ++c) {
        if (m_model->headerData(c, Qt::Horizontal).toString() == name)
            return c;
    }
    return -1;
}

int XYBackend::role(const char *name) const
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

void XYBackend::buildPoints()
{
    m_points.clear();
    m_xIsDate = false;
    m_sortedByX = false;

    const int valueRole = role("value");
    const int sourceRowRole = role("sourceRow");
    const int typeRole = role("columnType");
    const int yCol = columnIndex(m_yColumn);
    const int xCol = columnIndex(m_xColumn); // -1 = row number

    if (!m_model || valueRole < 0 || sourceRowRole < 0 || yCol < 0)
        return;
    if (xCol >= 0 && typeRole >= 0)
        m_xIsDate = m_model->headerData(xCol, Qt::Horizontal, typeRole).toInt() == kDateType;

    // Visit rows in file order, so "Row #" doesn't change when the table is sorted.
    const int rows = m_model->rowCount();
    QVector<QPair<int, int>> order; // (file row, model row)
    order.reserve(rows);
    for (int r = 0; r < rows; ++r)
        order.append({m_model->index(r, 0).data(sourceRowRole).toInt(), r});
    std::sort(order.begin(), order.end());

    m_points.reserve(rows);
    int shownRows = 0;
    for (const auto &entry : std::as_const(order)) {
        const int r = entry.second;
        ++shownRows; // counted even when Y is missing (matches GraphController)
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
        m_points.append(QPointF(x, yv.toDouble()));
    }

    if (xCol >= 0) {
        std::stable_sort(m_points.begin(), m_points.end(),
                         [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
        m_sortedByX = true;
    }
}

void XYBackend::updateRange()
{
    if (m_points.isEmpty()) {
        m_minX = 0.0; m_maxX = 1.0;
        m_minY = 0.0; m_maxY = 1.0;
        m_dataMinY = 0.0; m_dataMaxY = 1.0;
        return;
    }

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    for (const QPointF &p : std::as_const(m_points)) {
        minY = std::min(minY, p.y());
        maxY = std::max(maxY, p.y());
    }
    // Sorted by X or row numbers in order: first/last are the extremes.
    const double minX = m_points.first().x();
    const double maxX = m_points.last().x();

    const double spanX = maxX - minX;
    const double spanY = maxY - minY;
    const double padX = spanX > 0 ? spanX * 0.05 : (m_xIsDate ? 86400000.0 : 1.0);
    const double padY = spanY > 0 ? spanY * 0.05 : 1.0;

    m_minX = minX - padX; m_maxX = maxX + padX;
    m_minY = minY - padY; m_maxY = maxY + padY;
    m_dataMinY = minY; m_dataMaxY = maxY;
}
