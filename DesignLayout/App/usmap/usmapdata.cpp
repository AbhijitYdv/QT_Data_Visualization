#include "usmapdata.h"
#include "parserutils.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>

#include <cmath>

namespace UsMap {

// ---------------------------------------------------------- projection

namespace {

constexpr double kDeg = M_PI / 180.0;

struct Albers
{
    double n, c, rho0, lambda0;

    Albers()
    {
        const double phi1 = 29.5 * kDeg, phi2 = 45.5 * kDeg, phi0 = 37.5 * kDeg;
        lambda0 = -96.0 * kDeg;
        n = (std::sin(phi1) + std::sin(phi2)) / 2.0;
        c = std::cos(phi1) * std::cos(phi1) + 2.0 * n * std::sin(phi1);
        rho0 = std::sqrt(c - 2.0 * n * std::sin(phi0)) / n;
    }
};

const Albers &albers()
{
    static const Albers a;
    return a;
}

} // namespace

QPointF project(double lon, double lat)
{
    const Albers &a = albers();
    const double rho = std::sqrt(a.c - 2.0 * a.n * std::sin(lat * kDeg)) / a.n;
    const double theta = a.n * (lon * kDeg - a.lambda0);
    return {rho * std::sin(theta), a.rho0 - rho * std::cos(theta)};
}

bool inLower48(double lon, double lat)
{
    return lon >= -125.0 && lon <= -66.5 && lat >= 24.3 && lat <= 49.5;
}

// ------------------------------------------------------------ outlines

int Outline::stateAt(const QPointF &p) const
{
    for (int i = 0; i < states.size(); ++i) {
        const QPainterPath &path = states.at(i).path;
        if (path.controlPointRect().contains(p) && path.contains(p))
            return i;
    }
    return -1;
}

namespace {

void addRing(QPainterPath &path, const QJsonArray &ring)
{
    for (qsizetype i = 0; i < ring.size(); ++i) {
        const QJsonArray pos = ring.at(i).toArray();
        const QPointF p = project(pos.at(0).toDouble(), pos.at(1).toDouble());
        if (i == 0)
            path.moveTo(p);
        else
            path.lineTo(p);
    }
    path.closeSubpath();
}

} // namespace

Outline loadOutline(const QByteArray &geojson)
{
    Outline outline;
    const QJsonArray features = QJsonDocument::fromJson(geojson).object().value(u"features").toArray();

    for (const QJsonValue &f : features) {
        const QJsonObject feature = f.toObject();
        const QJsonObject geometry = feature.value(u"geometry").toObject();
        const QJsonObject props = feature.value(u"properties").toObject();
        const QString type = geometry.value(u"type").toString();

        QJsonArray polygons = geometry.value(u"coordinates").toArray();
        if (type == u"Polygon")
            polygons = QJsonArray{polygons};
        else if (type != u"MultiPolygon")
            continue;

        State state;
        state.name = props.value(u"name").toString();
        state.postal = props.value(u"postal").toString();
        state.path.setFillRule(Qt::OddEvenFill); // inner rings (lakes) become holes
        for (const QJsonValue &polygon : polygons)
            for (const QJsonValue &ring : polygon.toArray())
                addRing(state.path, ring.toArray());
        if (state.path.isEmpty())
            continue;

        outline.bounds = outline.bounds.isNull() ? state.path.boundingRect()
                                                 : outline.bounds.united(state.path.boundingRect());
        outline.states << state;
    }
    return outline;
}

const Outline &bundledOutline()
{
    static const Outline outline = [] {
        QFile file(QStringLiteral(":/maps/us-states.geojson"));
        return file.open(QIODevice::ReadOnly) ? loadOutline(file.readAll()) : Outline();
    }();
    return outline;
}

// ------------------------------------------------- latitude / longitude

bool columnInRange(const GraphTable &table, int column, double limit)
{
    if (column < 0 || column >= table.headers.size())
        return false;
    int numbers = 0, inRange = 0;
    const int sample = static_cast<int>(std::min<qsizetype>(table.rows.size(), 2000));
    for (int r = 0; r < sample; ++r) {
        double v = 0.0;
        if (!Parsers::parseNumber(table.rows.at(r).value(column), &v))
            continue;
        ++numbers;
        if (std::abs(v) <= limit)
            ++inRange;
    }
    return numbers > 0 && inRange * 10 >= numbers * 9;
}

namespace {

// "station.lat" -> {"station", "lat"}; "Start_Lng" -> {"start", "lng"};
// "Latitude" -> {"", "latitude"}
QPair<QString, QString> splitName(const QString &header)
{
    static const QRegularExpression separator(QStringLiteral("[._\\s-]+"));
    QStringList words = header.toLower().split(separator, Qt::SkipEmptyParts);
    if (words.isEmpty())
        return {};
    const QString last = words.takeLast();
    return {words.join(u'_'), last};
}

} // namespace

LatLonColumns detectLatLon(const GraphTable &table)
{
    static const QSet<QString> latNames{QStringLiteral("lat"), QStringLiteral("latitude")};
    static const QSet<QString> lonNames{QStringLiteral("lon"), QStringLiteral("lng"),
                                        QStringLiteral("long"), QStringLiteral("longitude")};
    const int number = static_cast<int>(CellType::Number);

    QVector<int> lats, lons;
    for (int c = 0; c < table.headers.size(); ++c) {
        if (table.types.value(c) != number)
            continue;
        const QString word = splitName(table.headers.at(c)).second;
        if (latNames.contains(word) && columnInRange(table, c, 90.0))
            lats << c;
        else if (lonNames.contains(word) && columnInRange(table, c, 180.0))
            lons << c;
    }

    LatLonColumns result;
    if (lats.isEmpty() || lons.isEmpty())
        return result;
    result.lat = lats.first();
    result.lon = lons.first();
    const QString prefix = splitName(table.headers.at(result.lat)).first;
    for (int c : std::as_const(lons)) {
        if (splitName(table.headers.at(c)).first == prefix) {
            result.lon = c;
            break;
        }
    }
    return result;
}

Points extractPoints(const GraphTable &table, const QVector<int> &rows, LatLonColumns columns)
{
    Points points;
    if (!columns.isValid())
        return points;
    points.lonLat.reserve(rows.size());
    points.rows.reserve(rows.size());

    for (int r : rows) {
        if (r < 0 || r >= table.rows.size())
            continue;
        const QStringList &row = table.rows.at(r);
        double lat = 0.0, lon = 0.0;
        if (!Parsers::parseNumber(row.value(columns.lat), &lat)
            || !Parsers::parseNumber(row.value(columns.lon), &lon)
            || std::abs(lat) > 90.0 || std::abs(lon) > 180.0) {
            ++points.missing;
            continue;
        }
        if (!inLower48(lon, lat)) {
            ++points.outside;
            continue;
        }
        points.lonLat << QPointF(lon, lat);
        points.rows << r;
    }
    return points;
}

} // namespace UsMap
