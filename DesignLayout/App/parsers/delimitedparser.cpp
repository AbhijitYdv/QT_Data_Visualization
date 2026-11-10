// CSV, TSV and whitespace-separated text.
//
// The delimiter is the one of tab, comma, semicolon or pipe used most often
// outside quotes in the first record (otherwise runs of whitespace). If the
// first record has no number or date in it, it is the header row. Lines
// starting with # before the header are comments. Quoted cells may contain
// delimiters and line breaks. Files that are not valid UTF-8 are read as
// Latin-1.

#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QRegularExpression>
#include <QStringDecoder>
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

// The candidate used most often outside quotes in the first record;
// none at all means "split on whitespace".
QChar guessDelimiter(const QString &record)
{
    const QList<QChar> candidates{u'\t', u',', u';', u'|'};
    QList<int> counts(candidates.size(), 0);
    bool inQuotes = false;
    for (QChar c : record) {
        if (c == u'"')
            inQuotes = !inQuotes;
        else if (!inQuotes)
            for (int i = 0; i < candidates.size(); ++i)
                counts[i] += c == candidates.at(i);
    }
    int best = -1;
    for (int i = 0; i < candidates.size(); ++i)
        if (counts.at(i) > 0 && (best < 0 || counts.at(i) > counts.at(best)))
            best = i;
    return best < 0 ? QChar() : candidates.at(best);
}

// One record: a line, plus the following lines while a quoted cell is still
// open (a quoted cell may contain line breaks).
QString readRecord(QTextStream &in)
{
    QString record = in.readLine();
    for (int extra = 0; record.count(u'"') % 2 == 1 && !in.atEnd() && extra < 10000; ++extra)
        record += u'\n' + in.readLine();
    return record;
}

// UTF-8 unless the start of the file is not valid UTF-8, in which case it is
// most likely Latin-1 / Windows-1252 from an older tool. Files with a byte
// order mark (UTF-8, UTF-16) are detected by QTextStream itself.
void chooseEncoding(QFile &file, QTextStream &in)
{
    const QByteArray head = file.peek(256 * 1024);
    if (head.startsWith("\xEF\xBB\xBF") || head.startsWith("\xFF\xFE") || head.startsWith("\xFE\xFF"))
        return;
    QStringDecoder utf8(QStringDecoder::Utf8);
    const QString decoded = utf8(head);
    Q_UNUSED(decoded)
    if (utf8.hasError())
        in.setEncoding(QStringConverter::Latin1);
}

} // namespace

GraphTable parseDelimited(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return errorTable(QCoreApplication::translate("Parsers", "Could not open the file"));

    GraphTable table;
    QTextStream in(&file);
    chooseEncoding(file, in);
    static const QRegularExpression anySeparator(QStringLiteral("[,;\\t|]+"));

    QChar delimiter;
    bool firstLine = true;
    int columnCount = 0;

    while (!in.atEnd()) {
        const QString line = readRecord(in).trimmed();
        if (line.isEmpty())
            continue;
        // '#' lines are comments only before the header ("# exported ...");
        // after that, "#1" is a value.
        if (firstLine && line.startsWith(u'#'))
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
