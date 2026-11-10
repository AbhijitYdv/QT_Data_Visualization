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

// "12", "-3.5", "1e6", "1,234,567" -> number. Empty text, NaN and infinity
// are not numbers.
bool parseNumber(const QString &text, double *value = nullptr);

// Dates and timestamps -> milliseconds since the epoch, UTC:
//   20221101, 2022-11-01, 2022/11/01           (date only: noon UTC)
//   2016-02-08 05:46:00, 2016-02-08T05:46:00Z, 2016-02-08T05:46:00.123+02:00
//   2/8/2016, 12/31/2016 23:30, 12/31/2016 11:30 PM  (US month/day/year)
// Times without a zone are taken as UTC.
bool parseDate(const QString &text, double *msecs = nullptr);

// Empty, or a common "no value" marker: NA, N/A, #N/A, NaN, null, None,
// nil, missing, -, --, ?. Such cells don't count when guessing a column's type.
bool isMissingValue(const QString &text);

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
