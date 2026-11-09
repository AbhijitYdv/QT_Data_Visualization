#include "parserutils.h"

#include <QCoreApplication>
#include <QDate>

#include <algorithm>

namespace Parsers {

namespace {
constexpr int kTypeSampleRows = 5000; // rows checked to guess column types
}

bool parseNumber(const QString &text, double *value)
{
    const QString t = text.trimmed();
    if (t.isEmpty())
        return false;
    bool ok = false;
    const double v = t.toDouble(&ok);
    if (ok && value)
        *value = v;
    return ok;
}

bool parseDate(const QString &text, double *msecs)
{
    const QString t = text.trimmed();
    int y = 0, m = 0, d = 0;
    bool okY = false, okM = false, okD = false;

    if (t.size() == 8) {
        y = t.left(4).toInt(&okY);
        m = t.mid(4, 2).toInt(&okM);
        d = t.mid(6, 2).toInt(&okD);
    } else if (t.size() == 10 && (t.at(4) == u'-' || t.at(4) == u'/') && t.at(7) == t.at(4)) {
        y = t.left(4).toInt(&okY);
        m = t.mid(5, 2).toInt(&okM);
        d = t.mid(8, 2).toInt(&okD);
    } else {
        return false;
    }

    if (!okY || !okM || !okD || y < 1900 || y > 2200)
        return false;
    const QDate date(y, m, d);
    if (!date.isValid())
        return false;

    if (msecs) {
        // Noon, so no time zone shows the previous day.
        constexpr qint64 kUnixEpochJulianDay = 2440588;
        constexpr qint64 kMsPerDay = 86400000;
        *msecs = static_cast<double>((date.toJulianDay() - kUnixEpochJulianDay) * kMsPerDay
                                     + kMsPerDay / 2);
    }
    return true;
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
            if (v.trimmed().isEmpty())
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
