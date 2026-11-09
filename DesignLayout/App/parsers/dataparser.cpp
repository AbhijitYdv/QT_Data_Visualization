#include "dataparser.h"
#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

namespace Parsers {

namespace {

using ParseFunction = GraphTable (*)(const QString &path);

struct Format
{
    const char *label;        // shown in the file dialog (translated)
    QStringList extensions;   // lower case, without the dot
    ParseFunction parse;
};

// Every supported format. Order = order in the file dialog.
const QList<Format> &formats()
{
    static const QList<Format> list{
        {QT_TRANSLATE_NOOP("Parsers", "CSV / text"), {"csv", "tsv", "txt"}, parseDelimited},
        {QT_TRANSLATE_NOOP("Parsers", "JSON"), {"json"}, parseJson},
        {QT_TRANSLATE_NOOP("Parsers", "JSON Lines"), {"jsonl", "ndjson"}, parseJsonLines},
        {QT_TRANSLATE_NOOP("Parsers", "GeoJSON"), {"geojson"}, parseGeoJson},
        {QT_TRANSLATE_NOOP("Parsers", "SQLite"), {"db", "sqlite", "sqlite3"}, parseSqlite},
        {QT_TRANSLATE_NOOP("Parsers", "XML"), {"xml"}, parseXml},
    };
    return list;
}

QString globs(const QStringList &extensions)
{
    QStringList out;
    for (const QString &ext : extensions)
        out << QStringLiteral("*.") + ext;
    return out.join(u' ');
}

// For files without a known extension: look at the first bytes.
ParseFunction sniff(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return parseDelimited; // it reports the open error itself

    const QByteArray head = file.peek(256);
    if (head.startsWith(QByteArrayLiteral("SQLite format 3")))
        return parseSqlite;

    QByteArray text = head;
    if (text.startsWith("\xEF\xBB\xBF")) // UTF-8 byte order mark
        text.remove(0, 3);
    text = text.trimmed();
    if (text.startsWith('{') || text.startsWith('['))
        return parseJson; // also detects GeoJSON and JSON Lines
    if (text.startsWith('<'))
        return parseXml;
    return parseDelimited;
}

} // namespace

GraphTable parseDataFile(const QString &path)
{
    const QFileInfo info(path);
    if (!info.isFile()) {
        return errorTable(QCoreApplication::translate("Parsers", "%1: file not found")
                              .arg(info.fileName()));
    }

    ParseFunction parse = nullptr;
    const QString suffix = info.suffix().toLower();
    for (const Format &format : formats()) {
        if (format.extensions.contains(suffix)) {
            parse = format.parse;
            break;
        }
    }
    if (!parse)
        parse = sniff(path);

    GraphTable table = parse(path);
    table.filePath = info.absoluteFilePath();
    table.fileName = table.section.isEmpty()
                         ? info.fileName()
                         : QStringLiteral("%1 – %2").arg(info.fileName(), table.section);
    if (!table.error.isEmpty())
        table.error = QStringLiteral("%1: %2").arg(info.fileName(), table.error);
    return table;
}

QStringList fileDialogFilters()
{
    QStringList all;
    QStringList filters;
    for (const Format &format : formats()) {
        all << format.extensions;
        filters << QStringLiteral("%1 (%2)")
                       .arg(QCoreApplication::translate("Parsers", format.label),
                            globs(format.extensions));
    }
    filters.prepend(QStringLiteral("%1 (%2)")
                        .arg(QCoreApplication::translate("Parsers", "All supported files"),
                             globs(all)));
    filters << QCoreApplication::translate("Parsers", "All files (*)");
    return filters;
}

} // namespace Parsers
