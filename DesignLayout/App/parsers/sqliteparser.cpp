// SQLite databases (.db, .sqlite, .sqlite3).
//
// Opens the file read-only and loads the table with the most rows (views are
// used only if there are no tables). The chosen table's name goes into
// GraphTable::section so the UI can show "shop.db – orders".
//
// Each call opens its own named connection and removes it again, so it is
// safe on the loader's worker thread and never shares a connection.

#include "formatparsers.h"

#include <QAtomicInteger>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace Parsers {

namespace {

QString tr(const char *text)
{
    return QCoreApplication::translate("Parsers", text);
}

QStringList userTables(const QSqlDatabase &db, QSql::TableType type)
{
    QStringList names = db.tables(type);
    names.removeIf([](const QString &name) { return name.startsWith(u"sqlite_"); });
    names.sort(Qt::CaseInsensitive);
    return names;
}

QString quoted(const QSqlDatabase &db, const QString &name)
{
    return db.driver()->escapeIdentifier(name, QSqlDriver::TableName);
}

// The table (or view) with the most rows; the first one alphabetically on a tie.
QString largestTable(const QSqlDatabase &db, const QStringList &names)
{
    QString best;
    qint64 bestRows = -1;
    for (const QString &name : names) {
        QSqlQuery count(db);
        if (!count.exec(QStringLiteral("SELECT COUNT(*) FROM %1").arg(quoted(db, name)))
            || !count.next()) {
            continue;
        }
        const qint64 rows = count.value(0).toLongLong();
        if (rows > bestRows) {
            best = name;
            bestRows = rows;
        }
    }
    return best;
}

QString cellText(const QVariant &value)
{
    if (value.isNull() || value.typeId() == QMetaType::QByteArray)
        return {}; // NULL or a BLOB: nothing to plot
    return value.toString();
}

GraphTable readTable(const QSqlDatabase &db)
{
    QStringList names = userTables(db, QSql::Tables);
    if (names.isEmpty())
        names = userTables(db, QSql::Views);
    if (names.isEmpty()) {
        const QString reason = db.lastError().isValid() ? db.lastError().text()
                                                        : tr("it has no tables");
        return errorTable(tr("Could not read the database: %1").arg(reason));
    }

    const QString name = largestTable(db, names);
    if (name.isEmpty())
        return errorTable(tr("Could not read any table in the database"));

    QSqlQuery query(db);
    query.setForwardOnly(true); // streams rows instead of caching them twice
    if (!query.exec(QStringLiteral("SELECT * FROM %1").arg(quoted(db, name))))
        return errorTable(tr("Could not read table %1: %2").arg(name, query.lastError().text()));

    GraphTable table;
    table.section = name;
    const QSqlRecord columns = query.record();
    const int columnCount = columns.count();
    for (int c = 0; c < columnCount; ++c)
        table.headers << columns.fieldName(c);

    while (query.next()) {
        QStringList row;
        row.reserve(columnCount);
        for (int c = 0; c < columnCount; ++c)
            row << cellText(query.value(c));
        table.rows << row;
    }
    return table;
}

} // namespace

GraphTable parseSqlite(const QString &path)
{
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")))
        return errorTable(tr("This Qt install has no SQLite driver (QSQLITE)"));

    static QAtomicInteger<int> nextId;
    const QString connection = QStringLiteral("Parsers.sqlite.%1").arg(nextId.fetchAndAddRelaxed(1));

    GraphTable table;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection);
        db.setDatabaseName(path);
        db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
        if (db.open())
            table = readTable(db);
        else
            table = errorTable(tr("Could not open the database: %1").arg(db.lastError().text()));
        db.close();
    } // `db` must be gone before the connection is removed
    QSqlDatabase::removeDatabase(connection);

    finishTable(table);
    return table;
}

} // namespace Parsers
