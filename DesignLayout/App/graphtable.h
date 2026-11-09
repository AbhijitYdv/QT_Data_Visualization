#ifndef GRAPHTABLE_H
#define GRAPHTABLE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

// What a column holds, stored per column in GraphTable::types.
// GraphController::ColumnType uses the same values.
enum class CellType : int { Text = 0, Number = 1, Date = 2 };

// One parsed dataset: the result of reading a file on the background thread.
// Copying a GraphTable is cheap: Qt containers are implicitly shared, so the
// history and the controller point at the same rows until one of them changes.
struct GraphTable
{
    QString fileName;
    QString filePath;           // full path, used to spot re-imports of the same file
    QString section;            // part of the file that was read, e.g. a SQLite table
    QString error;              // non-empty if loading failed
    QStringList headers;
    QList<QStringList> rows;
    QVector<int> types;         // one CellType per column
    int skippedLines = 0;
};

// The column choices last used for a dataset, stored by column name.
struct DatasetSelection
{
    QString xColumn;
    QString yColumn;
    QString filterColumn;
    QString filterValue;
};

#endif // GRAPHTABLE_H
