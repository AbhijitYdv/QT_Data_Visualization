#ifndef USMAPDATA_H
#define USMAPDATA_H

#include "graphtable.h"

#include <QByteArray>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>

// Everything the US map needs that is not drawing: the projection, the state
// outlines and finding latitude/longitude in a dataset. No QML here, so it
// is unit-tested on its own (tests/tst_usmap.cpp).
namespace UsMap {

// ---------------------------------------------------------- projection

// Albers equal-area conic, the usual projection for maps of the lower 48:
// standard parallels 29.5°N and 45.5°N, centred on 96°W, 37.5°N.
// Returns map units (unit sphere): x grows east, y grows NORTH.
QPointF project(double lon, double lat);

// Rough box around the lower 48 states. Points outside it are not drawn.
bool inLower48(double lon, double lat);

// ------------------------------------------------------------ outlines

struct State
{
    QString name;      // "New Jersey"
    QString postal;    // "NJ"
    QPainterPath path; // projected outline, map units (y north)
};

struct Outline
{
    QVector<State> states;
    QRectF bounds;     // of all states, map units

    // Index of the state containing map point `p`, or -1.
    int stateAt(const QPointF &p) const;
};

// Reads a GeoJSON FeatureCollection of (Multi)Polygons with "name" and
// "postal" properties. Returns an empty Outline if the data is not usable.
Outline loadOutline(const QByteArray &geojson);

// The lower 48 states + DC bundled with the app (Natural Earth 1:50m,
// public domain), loaded once.
const Outline &bundledOutline();

// ------------------------------------------------- latitude / longitude

struct LatLonColumns
{
    int lat = -1;
    int lon = -1;
    bool isValid() const { return lat >= 0 && lon >= 0; }
};

// Finds number columns named like latitude (lat, latitude) and longitude
// (lon, lng, long, longitude), also as the last part of a name such as
// "station.lat" or "start_lng", whose values are in range. Prefers a lon
// column with the same prefix as the lat column ("start_lat" + "start_lon").
LatLonColumns detectLatLon(const GraphTable &table);

// True if most values of `column` are numbers within [-limit, limit].
bool columnInRange(const GraphTable &table, int column, double limit);

struct Points
{
    QVector<QPointF> lonLat; // x = longitude, y = latitude
    QVector<int> rows;       // table row of each point
    int outside = 0;         // valid coordinates outside the lower 48
    int missing = 0;         // rows without usable coordinates
};

// Coordinates of `rows`, keeping only points inside the lower 48.
Points extractPoints(const GraphTable &table, const QVector<int> &rows, LatLonColumns columns);

} // namespace UsMap

#endif // USMAPDATA_H
