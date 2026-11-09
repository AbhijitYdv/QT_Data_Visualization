#include "parserutils.h"

#include <QCoreApplication>
#include <QDate>
#include <QRegularExpression>
#include <QSet>

#include <algorithm>
#include <cmath>

namespace Parsers {

namespace {
constexpr int kTypeSampleRows = 5000; // rows checked to guess column types
}

bool isMissingValue(const QString &text)
{
    static const QSet<QString> markers{
        QStringLiteral("na"),   QStringLiteral("n/a"),  QStringLiteral("#n/a"),
        QStringLiteral("nan"),  QStringLiteral("null"), QStringLiteral("none"),
        QStringLiteral("nil"),  QStringLiteral("missing"),
        QStringLiteral("-"),    QStringLiteral("--"),   QStringLiteral("?"),
    };
    const QString t = text.trimmed();
    return t.isEmpty() || (t.size() <= 7 && markers.contains(t.toLower()));
}

bool parseNumber(const QString &text, double *value)
{
    const QString t = text.trimmed();
    if (t.isEmpty())
        return false;
    bool ok = false;
    double v = t.toDouble(&ok);

    // "1,234,567.5": thousands separators, as spreadsheets export them.
    // "1,5" is not accepted, since it could be a European decimal comma.
    if (!ok && t.contains(u',')) {
        static const QRegularExpression thousands(
            QStringLiteral("^[-+]?\\d{1,3}(,\\d{3})+(\\.\\d+)?$"));
        if (thousands.match(t).hasMatch())
            v = QString(t).remove(u',').toDouble(&ok);
    }

    // "nan" and "inf" parse as doubles but are not values we can plot.
    if (!ok || !std::isfinite(v))
        return false;
    if (value)
        *value = v;
    return true;
}

namespace {

constexpr qint64 kUnixEpochJulianDay = 2440588;
constexpr qint64 kMsPerDay = 86400000;

// Milliseconds since the epoch, UTC. A date without a time is placed at noon
// so that no time zone shows it as the previous day.
bool toMsecs(int y, int m, int d, int hour, int minute, int second, int ms,
             bool hasTime, int offsetMinutes, double *msecs)
{
    if (y < 1900 || y > 2200)
        return false;
    const QDate date(y, m, d);
    if (!date.isValid() || hour > 23 || minute > 59 || second > 59)
        return false;
    if (msecs) {
        const qint64 day = (date.toJulianDay() - kUnixEpochJulianDay) * kMsPerDay;
        const qint64 time = hasTime ? ((hour * 60LL + minute - offsetMinutes) * 60 + second) * 1000 + ms
                                    : kMsPerDay / 2;
        *msecs = static_cast<double>(day + time);
    }
    return true;
}

int toInt(const QString &s)
{
    return s.isEmpty() ? 0 : s.toInt();
}

} // namespace

bool parseDate(const QString &text, double *msecs)
{
    const QString t = text.trimmed();
    if (t.size() < 6 || !t.front().isDigit())
        return false; // fast exit for words and short numbers

    // 20221101 (8 digits; "2/8/2016" is also 8 characters, so check digits)
    if (t.size() == 8) {
        bool digits = false;
        t.toInt(&digits);
        if (digits)
            return toMsecs(t.left(4).toInt(), t.mid(4, 2).toInt(), t.mid(6, 2).toInt(),
                           0, 0, 0, 0, false, 0, msecs);
    }

    // 2022-11-01, 2022/11/01, 2016-02-08 05:46:00, 2016-02-08T05:46:00.123Z,
    // 2016-02-08T05:46:00+02:00
    static const QRegularExpression iso(QStringLiteral(
        "^(\\d{4})([-/])(\\d{1,2})\\2(\\d{1,2})"
        "(?:[T ](\\d{1,2}):(\\d{2})(?::(\\d{2})(?:[.,](\\d{1,9}))?)?)?"
        "\\s*(Z|[+-]\\d{2}:?\\d{2})?$"));
    QRegularExpressionMatch m = iso.match(t);
    if (m.hasMatch()) {
        int offset = 0;
        const QString zone = m.captured(9);
        if (zone.size() > 1) { // +02:00 / -0500
            const QString digits = QString(zone.mid(1)).remove(u':');
            offset = digits.left(2).toInt() * 60 + digits.mid(2, 2).toInt();
            if (zone.front() == u'-')
                offset = -offset;
        }
        const QString frac = (m.captured(8) + QStringLiteral("00")).left(3);
        return toMsecs(m.captured(1).toInt(), m.captured(3).toInt(), m.captured(4).toInt(),
                       toInt(m.captured(5)), toInt(m.captured(6)), toInt(m.captured(7)),
                       m.captured(8).isEmpty() ? 0 : frac.toInt(),
                       m.capturedLength(5) > 0, offset, msecs);
    }

    // US month/day/year (the project reads ambiguous dates as US):
    // 2/8/2016, 12/31/2016 23:30, 12/31/2016 11:30 PM
    static const QRegularExpression us(QStringLiteral(
        "^(\\d{1,2})/(\\d{1,2})/(\\d{4})"
        "(?:[ T](\\d{1,2}):(\\d{2})(?::(\\d{2}))?\\s*([AaPp][Mm])?)?$"));
    m = us.match(t);
    if (m.hasMatch()) {
        int hour = toInt(m.captured(4));
        const QString ampm = m.captured(7).toLower();
        if (!ampm.isEmpty()) {
            if (hour < 1 || hour > 12)
                return false;
            hour = (hour % 12) + (ampm == u"pm" ? 12 : 0);
        }
        return toMsecs(m.captured(3).toInt(), m.captured(1).toInt(), m.captured(2).toInt(),
                       hour, toInt(m.captured(5)), toInt(m.captured(6)), 0,
                       m.capturedLength(4) > 0, 0, msecs);
    }
    return false;
}

// ------------------------------------------------------- RecordTableBuilder

void RecordTableBuilder::addRecord(const Record &record)
{
    QStringList row(m_headers.size());
    for (const auto &[name, value] : record) {
        auto it = m_columnOf.constFind(name);
        int column;
        if (it == m_columnOf.constEnd()) {
            column = static_cast<int>(m_headers.size());
            m_columnOf.insert(name, column);
            m_headers << name;
        } else {
            column = it.value();
        }
        if (column >= row.size())
            row.resize(column + 1);
        row[column] = value;
    }
    m_rows << row;
}

void RecordTableBuilder::moveInto(GraphTable &table)
{
    table.headers = std::move(m_headers);
    table.rows = std::move(m_rows);
    m_headers.clear();
    m_rows.clear();
    m_columnOf.clear();
}

// -------------------------------------------------------------- finishing

void finishTable(GraphTable &table)
{
    if (!table.error.isEmpty())
        return;
    if (table.rows.isEmpty() || table.headers.isEmpty()) {
        table.error = QCoreApplication::translate("Parsers", "No data rows found");
        return;
    }

    // Every row gets exactly one cell per column, so later code can index freely.
    const qsizetype columnCount = table.headers.size();
    for (QStringList &row : table.rows) {
        if (row.size() != columnCount)
            row.resize(columnCount);
    }

    // Guess each column's type from a sample of rows (90% rule).
    const int sample = std::min<int>(static_cast<int>(table.rows.size()), kTypeSampleRows);
    table.types = QVector<int>(columnCount, static_cast<int>(CellType::Text));
    for (int c = 0; c < columnCount; ++c) {
        int nonEmpty = 0, numbers = 0, dates = 0;
        for (int r = 0; r < sample; ++r) {
            const QString &v = table.rows.at(r).at(c);
            if (isMissingValue(v)) // empty, NA, N/A, null, - ...
                continue;
            ++nonEmpty;
            if (parseDate(v))
                ++dates;
            if (parseNumber(v))
                ++numbers;
        }
        if (nonEmpty == 0)
            continue;
        if (dates * 10 >= nonEmpty * 9)
            table.types[c] = static_cast<int>(CellType::Date);
        else if (numbers * 10 >= nonEmpty * 9)
            table.types[c] = static_cast<int>(CellType::Number);
    }
}

GraphTable errorTable(const QString &message)
{
    GraphTable table;
    table.error = message;
    return table;
}

} // namespace Parsers
