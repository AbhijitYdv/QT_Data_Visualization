// JSON and JSON Lines.
//
// Accepted shapes (checked in this order):
//   GeoJSON object                  -> handed to the GeoJSON parser
//   [ {..}, {..} ]                  -> one row per object (the usual case)
//   [ ["a","b"], [1,2] ]            -> rows of values; first row is the header if it is all text
//   [ 1, 2, 3 ]                     -> one column called "value"
//   { "x": [..], "y": [..] }        -> one column per key
//   { "data": [ {..}, .. ], .. }    -> the largest array of objects found (up to 3 levels deep)
//   { "a": 1, "b": {..} }           -> a single row
//
// Nested objects become dotted column names ("location.lat"); arrays inside
// a record are kept as compact JSON text.
//
// Note: QJsonObject keeps keys in alphabetical order, so the columns come out
// sorted rather than in the order they appear in the file.

#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLocale>

#include <algorithm>

namespace Parsers {

namespace {

QString tr(const char *text)
{
    return QCoreApplication::translate("Parsers", text);
}

QString scalarText(const QJsonValue &value)
{
    switch (value.type()) {
    case QJsonValue::Bool:
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    case QJsonValue::Double:
        return QString::number(value.toDouble(), 'g', QLocale::FloatingPointShortest);
    case QJsonValue::String:
        return value.toString();
    case QJsonValue::Array:
        return QString::fromUtf8(QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact));
    case QJsonValue::Object:
        return QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
    case QJsonValue::Null:
    case QJsonValue::Undefined:
        break;
    }
    return {};
}

bool isScalar(const QJsonValue &value)
{
    return !value.isArray() && !value.isObject();
}

// { "x": [1,2], "y": [3,4] }: every value is an array of plain values.
bool isColumnar(const QJsonObject &object)
{
    if (object.isEmpty())
        return false;
    bool anyValues = false;
    for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
        if (!it.value().isArray())
            return false;
        const QJsonArray column = it.value().toArray();
        for (const QJsonValue &cell : column) {
            if (!isScalar(cell))
                return false;
        }
        anyValues = anyValues || !column.isEmpty();
    }
    return anyValues;
}

GraphTable columnarTable(const QJsonObject &object)
{
    GraphTable table;
    qsizetype rowCount = 0;
    for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
        table.headers << it.key();
        rowCount = std::max(rowCount, it.value().toArray().size());
    }

    table.rows.reserve(rowCount);
    for (qsizetype r = 0; r < rowCount; ++r) {
        QStringList row;
        row.reserve(table.headers.size());
        for (auto it = object.constBegin(); it != object.constEnd(); ++it)
            row << scalarText(it.value().toArray().at(r)); // out of range -> Undefined -> ""
        table.rows << row;
    }
    return table;
}

GraphTable recordsTable(const QJsonArray &array)
{
    RecordTableBuilder builder;
    for (const QJsonValue &element : array) {
        Record record;
        flattenJson(element, QString(), record);
        builder.addRecord(record);
    }
    GraphTable table;
    builder.moveInto(table);
    return table;
}

// [ ["date","price"], ["2022-11-01", 1450], ... ]
GraphTable rowsTable(const QJsonArray &array)
{
    GraphTable table;
    qsizetype columnCount = 0;
    for (const QJsonValue &row : array)
        columnCount = std::max(columnCount, row.toArray().size());

    auto allText = [](const QJsonArray &row) {
        return std::all_of(row.begin(), row.end(),
                           [](const QJsonValue &v) { return v.isString(); });
    };

    qsizetype first = 0;
    const QJsonArray firstRow = array.at(0).toArray();
    const bool hasHeader = array.size() > 1 && allText(firstRow)
                           && !allText(array.at(1).toArray());
    if (hasHeader) {
        for (const QJsonValue &name : firstRow)
            table.headers << name.toString();
        first = 1;
    }
    for (qsizetype c = table.headers.size(); c < columnCount; ++c)
        table.headers << tr("Column %1").arg(c + 1);

    for (qsizetype r = first; r < array.size(); ++r) {
        const QJsonValue value = array.at(r);
        if (!value.isArray()) {
            ++table.skippedLines;
            continue;
        }
        QStringList row;
        for (const QJsonValue &cell : value.toArray())
            row << scalarText(cell);
        table.rows << row;
    }
    return table;
}

GraphTable arrayTable(const QJsonArray &array)
{
    if (array.isEmpty())
        return errorTable(tr("The JSON array is empty"));

    int objects = 0, arrays = 0;
    for (const QJsonValue &v : array) {
        objects += v.isObject();
        arrays += v.isArray();
    }
    if (objects * 2 >= array.size())
        return recordsTable(array);
    if (arrays * 2 >= array.size())
        return rowsTable(array);

    GraphTable table; // plain values
    table.headers << QStringLiteral("value");
    for (const QJsonValue &v : array)
        table.rows << QStringList{scalarText(v)};
    return table;
}

// Finds the largest array of objects inside `object`, searching a few levels down.
bool findRecordArray(const QJsonObject &object, int depth, QJsonArray *best)
{
    bool found = false;
    for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
        const QJsonValue value = it.value();
        if (value.isArray()) {
            const QJsonArray array = value.toArray();
            const bool hasObjects = !array.isEmpty() && array.first().isObject();
            if (hasObjects && array.size() > best->size()) {
                *best = array;
                found = true;
            }
        } else if (value.isObject() && depth > 0) {
            found = findRecordArray(value.toObject(), depth - 1, best) || found;
        }
    }
    return found;
}

GraphTable tableFromDocument(const QJsonDocument &document)
{
    if (document.isArray())
        return arrayTable(document.array());

    const QJsonObject root = document.object();
    if (root.isEmpty())
        return errorTable(tr("The JSON object is empty"));
    if (looksLikeGeoJson(root))
        return geoJsonTable(root);
    if (isColumnar(root))
        return columnarTable(root);

    QJsonArray records;
    if (findRecordArray(root, 3, &records))
        return recordsTable(records);

    return recordsTable(QJsonArray{root}); // a single record
}

} // namespace

void flattenJson(const QJsonValue &value, const QString &name, Record &out)
{
    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            const QString child = name.isEmpty() ? it.key() : name + u'.' + it.key();
            flattenJson(it.value(), child, out);
        }
        return;
    }
    out << qMakePair(name.isEmpty() ? QStringLiteral("value") : name, scalarText(value));
}

GraphTable parseJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return errorTable(tr("Could not open the file"));

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error == QJsonParseError::GarbageAtEnd)
        return parseJsonLines(path); // one object per line, saved as .json
    if (error.error != QJsonParseError::NoError) {
        return errorTable(tr("Invalid JSON at character %1: %2")
                              .arg(error.offset)
                              .arg(error.errorString()));
    }

    GraphTable table = tableFromDocument(document);
    finishTable(table);
    return table;
}

GraphTable parseJsonLines(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return errorTable(tr("Could not open the file"));

    RecordTableBuilder builder;
    int badLines = 0;
    int lineNumber = 0;
    int firstBadLine = 0;

    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        ++lineNumber;
        if (line.isEmpty())
            continue;

        QJsonParseError error;
        const QJsonDocument document = QJsonDocument::fromJson(line, &error);
        if (error.error != QJsonParseError::NoError || !document.isObject()) {
            if (++badLines == 1)
                firstBadLine = lineNumber;
            continue;
        }
        Record record;
        flattenJson(document.object(), QString(), record);
        builder.addRecord(record);
    }

    if (builder.recordCount() == 0 && badLines > 0)
        return errorTable(tr("Invalid JSON on line %1").arg(firstBadLine));

    GraphTable table;
    builder.moveInto(table);
    table.skippedLines = badLines;
    finishTable(table);
    return table;
}

} // namespace Parsers
