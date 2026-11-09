// CSV, TSV and whitespace-separated text.
//
// The delimiter is taken from the first data line (tab, then comma, then
// semicolon, otherwise runs of whitespace). If the first line has no number
// or date in it, it is used as the header row. Lines starting with # are
// comments.

#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

#include <algorithm>

namespace Parsers {

namespace {

// Splits one line on a single-character delimiter, respecting "quoted, fields".
// A null delimiter means "split on runs of whitespace".
QStringList splitLine(const QString &line, QChar delimiter)
{
    if (delimiter.isNull()) {
        static const QRegularExpression ws(QStringLiteral("\\s+"));
        return line.split(ws, Qt::SkipEmptyParts);
    }

    QStringList fields;
    QString field;
    bool inQuotes = false;

    for (qsizetype i = 0; i < line.size(); ++i) {
        const QChar c = line.at(i);
        if (inQuotes) {
            if (c == u'"') {
                if (i + 1 < line.size() && line.at(i + 1) == u'"') {
                    field += u'"';
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else if (c == u'"') {
            inQuotes = true;
        } else if (c == delimiter) {
            fields << field.trimmed();
            field.clear();
        } else {
            field += c;
        }
    }
    fields << field.trimmed();
    return fields;
}

QChar guessDelimiter(const QString &line)
{
    if (line.contains(u'\t'))
        return u'\t';
    if (line.contains(u','))
        return u',';
    if (line.contains(u';'))
        return u';';
    return {}; // whitespace
}

} // namespace

GraphTable parseDelimited(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return errorTable(QCoreApplication::translate("Parsers", "Could not open the file"));

    GraphTable table;
    QTextStream in(&file);
    static const QRegularExpression anySeparator(QStringLiteral("[,;\\t]+"));

    QChar delimiter;
    bool firstLine = true;
    int columnCount = 0;

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(u'#'))
            continue;

        if (firstLine) {
            firstLine = false;
            delimiter = guessDelimiter(line);

            const QStringList fields = splitLine(line, delimiter);
            const bool isHeader = std::none_of(fields.cbegin(), fields.cend(),
                                               [](const QString &f) {
                                                   return parseNumber(f) || parseDate(f);
                                               });
            columnCount = static_cast<int>(fields.size());
            if (isHeader) {
                table.headers = fields;
                continue;
            }
            for (int c = 0; c < columnCount; ++c)
                table.headers << QCoreApplication::translate("Parsers", "Column %1").arg(c + 1);
            table.rows << fields;
            continue;
        }

        QStringList fields = splitLine(line, delimiter);
        if (fields.size() < 2 && columnCount >= 2)
            fields = line.split(anySeparator, Qt::SkipEmptyParts);
        if (fields.size() < 2 && columnCount >= 2) {
            ++table.skippedLines;
            continue;
        }
        table.rows << fields;
    }

    finishTable(table);
    return table;
}

} // namespace Parsers
