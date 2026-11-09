#ifndef PARSERUTILS_H
#define PARSERUTILS_H

#include "graphtable.h"

#include <QHash>
#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

// Helpers shared by every format parser. Nothing in here touches a QObject,
// so it is all safe to call from the loader's worker thread.
namespace Parsers {

// One record as (column name, value) pairs, in the order they were read.
using Record = QList<QPair<QString, QString>>;

// "12", "-3.5", "1e6" -> number. Empty text is not a number.
bool parseNumber(const QString &text, double *value = nullptr);

// 20221101, 2022-11-01 or 2022/11/01 -> milliseconds since the epoch at noon UTC.
bool parseDate(const QString &text, double *msecs = nullptr);

// Builds a table from records that may not all have the same fields
// (JSON objects, XML elements, GeoJSON features). Columns appear in the
// order they are first seen; a field missing from a record becomes "".
class RecordTableBuilder
{
public:
    void addRecord(const Record &record);
    int recordCount() const { return static_cast<int>(m_rows.size()); }

    // Moves the headers and rows into `table` and resets the builder.
    void moveInto(GraphTable &table);

private:
    QStringList m_headers;
    QHash<QString, int> m_columnOf;
    QList<QStringList> m_rows;
};

// Last step of every parser: sets an error if there are no rows, pads short
// rows, and guesses each column's CellType from a sample of rows.
void finishTable(GraphTable &table);

// A table that only carries an error message for the user.
GraphTable errorTable(const QString &message);

} // namespace Parsers

#endif // PARSERUTILS_H
