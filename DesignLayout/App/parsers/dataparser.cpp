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

QString tr(const char *text)
{
    return QCoreApplication::translate("Parsers", text);
}

// Spreadsheets, archives and other binary files: a message saying what the
// file is and what to do, instead of reading it as garbage text.
// Empty if the file looks like text (or SQLite).
QString binaryFileError(const QByteArray &head, const QString &suffix)
{
    if (head.startsWith("SQLite format 3"))
        return {};
    if (head.startsWith("PK\x03\x04")) { // zip container
        if (suffix.startsWith(u"xl"))
            return tr("Excel workbooks can't be read yet. In Excel, use File > Save As > "
                      "CSV, then load the CSV.");
        if (suffix == u"ods")
            return tr("OpenDocument spreadsheets can't be read yet. Save the sheet as CSV, "
                      "then load the CSV.");
        return tr("This is a zip archive (or an Excel/Office file). Unzip it or save it as "
                  "CSV, then load that.");
    }
    if (head.startsWith("\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1"))
        return tr("Old Excel (.xls) workbooks can't be read. In Excel, use File > Save As > "
                  "CSV, then load the CSV.");
    if (head.startsWith("PAR1"))
        return tr("Parquet files aren't supported yet. Export the data to CSV first.");
    if (head.startsWith("\x1F\x8B"))
        return tr("This file is gzip-compressed. Decompress it first, then load the result.");
    if (head.startsWith("%PDF"))
        return tr("PDF files can't be read as data. Export the table to CSV first.");

    // UTF-16 text has zero bytes too, but starts with a byte order mark.
    const bool utf16 = head.startsWith("\xFF\xFE") || head.startsWith("\xFE\xFF");
    if (!utf16 && head.contains('\0'))
        return tr("This doesn't look like a text data file.");
    return {};
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

    const QString suffix = info.suffix().toLower();
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            const QString binary = binaryFileError(file.peek(4096), suffix);
            if (!binary.isEmpty())
                return errorTable(QStringLiteral("%1: %2").arg(info.fileName(), binary));
        }
    }

    ParseFunction parse = nullptr;
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
