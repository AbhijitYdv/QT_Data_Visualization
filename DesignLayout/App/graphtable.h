#ifndef GRAPHTABLE_H
#define GRAPHTABLE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

// One parsed dataset: the result of reading a file on the background thread.
// Copying a GraphTable is cheap: Qt containers are implicitly shared, so the
// history and the controller point at the same rows until one of them changes.
struct GraphTable
{
    QString fileName;
    QString filePath;           // full path, used to spot re-imports of the same file
    QString error;              // non-empty if loading failed
    QStringList headers;
    QList<QStringList> rows;
    QVector<int> types;         // one of GraphController::ColumnType per column
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
