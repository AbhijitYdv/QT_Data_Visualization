// GeoJSON (RFC 7946).
//
// One row per feature:
//   id                         - the feature's id, if it has one
//   <property names>           - from "properties" (nested objects -> dotted names)
//   geometry                   - Point, Polygon, ...
//   longitude, latitude        - the point itself, or the average of all the
//                                geometry's positions for lines and shapes
//   altitude                   - only if a Point has a third coordinate
//
// A bare geometry or a single Feature becomes a one-row table.

#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

namespace Parsers {

namespace {

QString tr(const char *text)
{
    return QCoreApplication::translate("Parsers", text);
}

const QSet<QString> &geometryTypes()
{
    static const QSet<QString> types{
        QStringLiteral("Point"),           QStringLiteral("MultiPoint"),
        QStringLiteral("LineString"),      QStringLiteral("MultiLineString"),
        QStringLiteral("Polygon"),         QStringLiteral("MultiPolygon"),
        QStringLiteral("GeometryCollection"),
    };
    return types;
}

// Adds up every [lon, lat] position found anywhere inside `coordinates`.
void sumPositions(const QJsonValue &coordinates, double *lon, double *lat, int *count)
{
    const QJsonArray array = coordinates.toArray();
    if (array.size() >= 2 && array.at(0).isDouble() && array.at(1).isDouble()) {
        *lon += array.at(0).toDouble();
        *lat += array.at(1).toDouble();
        ++*count;
        return;
    }
    for (const QJsonValue &inner : array)
        sumPositions(inner, lon, lat, count);
}

void sumGeometry(const QJsonObject &geometry, double *lon, double *lat, int *count)
{
    if (geometry.value(u"type").toString() == u"GeometryCollection") {
        for (const QJsonValue &part : geometry.value(u"geometries").toArray())
            sumGeometry(part.toObject(), lon, lat, count);
        return;
    }
    sumPositions(geometry.value(u"coordinates"), lon, lat, count);
}

QString number(double value)
{
    return QString::number(value, 'g', 12);
}

void addGeometry(const QJsonObject &geometry, Record &record)
{
    const QString type = geometry.value(u"type").toString();
    record << qMakePair(QStringLiteral("geometry"), type);

    const QJsonArray point = geometry.value(u"coordinates").toArray();
    if (type == u"Point" && point.size() >= 2) {
        record << qMakePair(QStringLiteral("longitude"), number(point.at(0).toDouble()))
               << qMakePair(QStringLiteral("latitude"), number(point.at(1).toDouble()));
        if (point.size() >= 3)
            record << qMakePair(QStringLiteral("altitude"), number(point.at(2).toDouble()));
        return;
    }

    double lon = 0.0, lat = 0.0;
    int count = 0;
    sumGeometry(geometry, &lon, &lat, &count);
    if (count > 0) {
        record << qMakePair(QStringLiteral("longitude"), number(lon / count))
               << qMakePair(QStringLiteral("latitude"), number(lat / count));
    }
}

Record featureRecord(const QJsonObject &feature)
{
    Record record;
    if (feature.contains(u"id"))
        flattenJson(feature.value(u"id"), QStringLiteral("id"), record);
    flattenJson(feature.value(u"properties").toObject(), QString(), record);
    const QJsonValue geometry = feature.value(u"geometry");
    if (geometry.isObject())
        addGeometry(geometry.toObject(), record);
    return record;
}

} // namespace

bool looksLikeGeoJson(const QJsonObject &object)
{
    const QString type = object.value(u"type").toString();
    if (type == u"FeatureCollection")
        return object.value(u"features").isArray();
    if (type == u"Feature")
        return object.contains(u"geometry") || object.contains(u"properties");
    return geometryTypes().contains(type)
           && (object.contains(u"coordinates") || object.contains(u"geometries"));
}

GraphTable geoJsonTable(const QJsonObject &root)
{
    QJsonArray features;
    const QString type = root.value(u"type").toString();
    if (type == u"FeatureCollection")
        features = root.value(u"features").toArray();
    else if (type == u"Feature")
        features << root;
    else // a bare geometry
        features << QJsonObject{{QStringLiteral("type"), QStringLiteral("Feature")},
                                {QStringLiteral("geometry"), root}};

    RecordTableBuilder builder;
    GraphTable table;
    for (const QJsonValue &feature : features) {
        if (!feature.isObject()) {
            ++table.skippedLines;
            continue;
        }
        builder.addRecord(featureRecord(feature.toObject()));
    }
    builder.moveInto(table);
    return table;
}

GraphTable parseGeoJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return errorTable(tr("Could not open the file"));

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        return errorTable(tr("Invalid JSON at character %1: %2")
                              .arg(error.offset)
                              .arg(error.errorString()));
    }
    if (!document.isObject() || !looksLikeGeoJson(document.object()))
        return errorTable(tr("No GeoJSON FeatureCollection, Feature or geometry found"));

    GraphTable table = geoJsonTable(document.object());
    finishTable(table);
    return table;
}

} // namespace Parsers
