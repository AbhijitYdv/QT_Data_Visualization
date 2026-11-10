#ifndef DATAPARSER_H
#define DATAPARSER_H

#include "graphtable.h"

#include <QString>
#include <QStringList>

// The one entry point the app uses to read a data file.
//
// Supported: CSV / TSV / TXT, JSON, JSON Lines, GeoJSON, SQLite and XML.
// To add a format: write `GraphTable parseXyz(const QString &path)` in its own
// file, declare it in formatparsers.h and add one line to the table in
// dataparser.cpp.
namespace Parsers {

// Reads `path` and returns the table, or a table whose `error` says what went
// wrong (prefixed with the file name). The format is picked by extension; an
// unknown extension is recognised from the first bytes of the file.
// Safe to run on a worker thread.
GraphTable parseDataFile(const QString &path);

// Filters for a QML FileDialog's nameFilters: "All supported" first, then one
// entry per format, then "All files".
QStringList fileDialogFilters();

} // namespace Parsers

#endif // DATAPARSER_H
