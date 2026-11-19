#include "mapbackend.h"
#include "modelaccess.h"
#include "usmapdata.h"
#include "usmapview.h"

#include <QMetaObject>

#include <algorithm>
#include <cmath>

MapBackend::MapBackend(QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    ModelAccess::onAnyChange(m_model, this, [this] { scheduleRefresh(); });
    scheduleRefresh();
}

void MapBackend::setLatColumn(const QString &name)
{
    if (name == m_lat || !m_numberColumns.contains(name))
        return;
    m_lat = name;
    scheduleRefresh();
}

void MapBackend::setLonColumn(const QString &name)
{
    if (name == m_lon || !m_numberColumns.contains(name))
        return;
    m_lon = name;
    scheduleRefresh();
}

void MapBackend::scheduleRefresh()
{
    if (m_queued)
        return;
    m_queued = true;
    QMetaObject::invokeMethod(this, [this] { refresh(); }, Qt::QueuedConnection);
}

// Same rule as UsMap::columnInRange: of the first 2000 numbers, at least 90%
// are within [-limit, limit].
bool MapBackend::columnInRange(int column, double limit) const
{
    const int valueRole = ModelAccess::role(m_model, "value");
    if (!m_model || valueRole < 0)
        return false;
    int numbers = 0, inRange = 0;
    const int sample = std::min(m_model->rowCount(), 2000);
    for (int r = 0; r < sample; ++r) {
        const QVariant v = m_model->index(r, column).data(valueRole);
        if (!v.isValid())
            continue;
        ++numbers;
        if (std::abs(v.toDouble()) <= limit)
            ++inRange;
    }
    return numbers > 0 && inRange * 10 >= numbers * 9;
}

void MapBackend::refresh()
{
    m_queued = false;
    m_lonLat.clear();
    m_rows.clear();
    m_outside = 0;
    m_missing = 0;

    const QStringList headers = ModelAccess::headers(m_model);
    m_numberColumns = ModelAccess::columnsOfType(m_model, ModelAccess::Number);

    // New dataset (or columns gone): detect the lat/lon columns again.
    if (headers != m_lastHeaders || !m_numberColumns.contains(m_lat)
        || !m_numberColumns.contains(m_lon)) {
        m_lastHeaders = headers;
        const UsMap::LatLonColumns found = UsMap::detectLatLon(
            headers, ModelAccess::types(m_model),
            [this](int column, double limit) { return columnInRange(column, limit); });
        m_lat = found.isValid() ? headers.at(found.lat) : QString();
        m_lon = found.isValid() ? headers.at(found.lon) : QString();
    }

    const int valueRole = ModelAccess::role(m_model, "value");
    const int latCol = ModelAccess::column(m_model, m_lat);
    const int lonCol = ModelAccess::column(m_model, m_lon);
    if (m_model && valueRole >= 0 && latCol >= 0 && lonCol >= 0) {
        const int rows = m_model->rowCount();
        m_lonLat.reserve(rows);
        m_rows.reserve(rows);
        for (int r = 0; r < rows; ++r) {
            const QVariant latV = m_model->index(r, latCol).data(valueRole);
            const QVariant lonV = m_model->index(r, lonCol).data(valueRole);
            const double lat = latV.toDouble(), lon = lonV.toDouble();
            if (!latV.isValid() || !lonV.isValid() || std::abs(lat) > 90.0
                || std::abs(lon) > 180.0) {
                ++m_missing;
                continue;
            }
            if (!UsMap::inLower48(lon, lat)) {
                ++m_outside;
                continue;
            }
            m_lonLat << QPointF(lon, lat);
            m_rows << r;
        }
    }

    emit changed();
}

void MapBackend::fillMap(QObject *item) const
{
    if (auto *map = qobject_cast<UsMapView *>(item))
        map->setPoints(m_lonLat, m_rows);
}

QString MapBackend::rowSummary(int modelRow) const
{
    if (!m_model || modelRow < 0 || modelRow >= m_model->rowCount())
        return {};
    const QStringList headers = ModelAccess::headers(m_model);
    QStringList lines;
    for (int c = 0; c < headers.size() && lines.size() < 5; ++c) {
        if (headers.at(c) == m_lat || headers.at(c) == m_lon)
            continue;
        const QString cell = m_model->index(modelRow, c).data().toString();
        if (cell.trimmed().isEmpty())
            continue;
        lines << QStringLiteral("%1: %2").arg(headers.at(c), cell);
    }
    return lines.join(u'\n');
}
