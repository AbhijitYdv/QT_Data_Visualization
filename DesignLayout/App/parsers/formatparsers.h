#ifndef FORMATPARSERS_H
#define FORMATPARSERS_H

#include "graphtable.h"
#include "parserutils.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

// One function per file format. Only dataparser.cpp (and the format files
// themselves) include this header; the rest of the app calls parseDataFile().
//
// Every function:
//  - is safe to run on a worker thread (no QObject / UI access);
//  - fills headers, rows and types, or sets GraphTable::error;
//  - leaves fileName / filePath to the caller.
namespace Parsers {

GraphTable parseDelimited(const QString &path); // .csv .tsv .txt
GraphTable parseJson(const QString &path);      // .json
GraphTable parseJsonLines(const QString &path); // .jsonl .ndjson
GraphTable parseGeoJson(const QString &path);   // .geojson
GraphTable parseSqlite(const QString &path);    // .db .sqlite .sqlite3
GraphTable parseXml(const QString &path);       // .xml

// Shared between the JSON and GeoJSON parsers.

// Appends `value` to `out`, turning nested objects into dotted names
// ({"a": {"b": 1}} -> "a.b" = "1"). Arrays are kept as compact JSON text.
void flattenJson(const QJsonValue &value, const QString &name, Record &out);

// True for a FeatureCollection, a Feature or a bare geometry object.
bool looksLikeGeoJson(const QJsonObject &object);

// Builds the table for an already-parsed GeoJSON object.
GraphTable geoJsonTable(const QJsonObject &root);

} // namespace Parsers

#endif // FORMATPARSERS_H
