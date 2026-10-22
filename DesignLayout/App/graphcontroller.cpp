#include "graphcontroller.h"

#include <QDate>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>
#include <QTime>

#include <algorithm>
#include <limits>

namespace {
const QString kRowNumber = QStringLiteral("Row #");
const QString kNoFilter = QStringLiteral("(none)");
constexpr int kMaxFilterValues = 500;
} // namespace

GraphController::GraphController(QObject *parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------
// Parsing helpers
// ---------------------------------------------------------------------------

bool GraphController::parseNumber(const QString &text, double *value)
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

bool GraphController::parseDate(const QString &text, double *msecs)
{
    const QString t = text.trimmed();
    QDate date;

    if (t.size() == 8) {
        bool allDigits = true;
        for (const QChar c : t)
            allDigits = allDigits && c.isDigit();
        if (allDigits)
            date = QDate::fromString(t, QStringLiteral("yyyyMMdd"));
    } else if (t.size() == 10) {
        date = QDate::fromString(t, Qt::ISODate); // 2022-11-01
        if (!date.isValid())
            date = QDate::fromString(t, QStringLiteral("yyyy/MM/dd"));
    }

    if (!date.isValid() || date.year() < 1900 || date.year() > 2200)
        return false;

    // Noon local time, so the date axis never shows the previous day.
    if (msecs)
        *msecs = static_cast<double>(QDateTime(date, QTime(12, 0)).toMSecsSinceEpoch());
    return true;
}

// Splits one line on a single-character delimiter, respecting "quoted, fields".
// A null delimiter means "split on runs of whitespace".
QStringList GraphController::splitLine(const QString &line, QChar delimiter)
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
                    field += u'"'; // escaped quote ""
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

// ---------------------------------------------------------------------------
// Loading
// ---------------------------------------------------------------------------

bool GraphController::loadFile(const QUrl &fileUrl)
{
    const QString path = fileUrl.isLocalFile() ? fileUrl.toLocalFile()
                                               : fileUrl.toString();
    const QString name = QFileInfo(path).fileName();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStatus(tr("Could not open %1").arg(name));
        return false;
    }

    // Read all non-empty, non-comment lines (QTextStream reads UTF-8,
    // so Japanese and other non-ASCII text works).
    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith(u'#'))
            lines << line;
    }
    if (lines.isEmpty()) {
        setStatus(tr("%1 is empty").arg(name));
        return false;
    }

    // Pick the delimiter from the first line.
    const QString &first = lines.first();
    QChar delimiter; // null = whitespace
    if (first.contains(u'\t'))
        delimiter = u'\t';
    else if (first.contains(u','))
        delimiter = u',';
    else if (first.contains(u';'))
        delimiter = u';';

    static const QRegularExpression anySeparator(QStringLiteral("[,;\\t]+"));

    // Header row = first row where no field is a number or a date.
    QStringList firstFields = splitLine(first, delimiter);
    bool hasHeader = true;
    for (const QString &f : firstFields) {
        if (parseNumber(f, nullptr) || parseDate(f, nullptr)) {
            hasHeader = false;
            break;
        }
    }

    QStringList headers;
    int startLine = 0;
    if (hasHeader) {
        headers = firstFields;
        startLine = 1;
    } else {
        for (int c = 0; c < firstFields.size(); ++c)
            headers << tr("Column %1").arg(c + 1);
    }
    const int columnCount = static_cast<int>(headers.size());

    QList<QStringList> rows;
    int skipped = 0;
    for (int i = startLine; i < lines.size(); ++i) {
        QStringList fields = splitLine(lines.at(i), delimiter);
        // A line using a different separator than the rest of the file.
        if (fields.size() < 2 && columnCount >= 2)
            fields = lines.at(i).split(anySeparator, Qt::SkipEmptyParts);
        if (fields.size() < 2 && columnCount >= 2) {
            ++skipped;
            continue;
        }
        rows << fields;
    }

    if (rows.isEmpty()) {
        setStatus(tr("No data rows found in %1").arg(name));
        return false;
    }

    m_headers = headers;
    m_rows = rows;
    m_fileName = name;
    m_skippedLines = skipped;

    detectColumnTypes();
    pickDefaultColumns();
    refreshFilterValues();

    emit tableChanged();
    emit selectionChanged();
    rebuildPoints();
    return true;
}

void GraphController::detectColumnTypes()
{
    const int columnCount = static_cast<int>(m_headers.size());
    m_types = QVector<ColumnType>(columnCount, ColumnType::Text);

    for (int c = 0; c < columnCount; ++c) {
        int nonEmpty = 0, numbers = 0, dates = 0;
        for (const QStringList &row : std::as_const(m_rows)) {
            const QString v = row.value(c).trimmed();
            if (v.isEmpty())
                continue;
            ++nonEmpty;
            if (parseDate(v, nullptr))
                ++dates;
            if (parseNumber(v, nullptr))
                ++numbers;
        }
        if (nonEmpty == 0)
            continue;
        // 90% rule tolerates a few bad cells.
        if (dates * 10 >= nonEmpty * 9)
            m_types[c] = ColumnType::Date;
        else if (numbers * 10 >= nonEmpty * 9)
            m_types[c] = ColumnType::Number;
    }
}

void GraphController::pickDefaultColumns()
{
    const int columnCount = static_cast<int>(m_types.size());
    QList<int> numberCols, dateCols, textCols;
    for (int c = 0; c < columnCount; ++c) {
        switch (m_types.at(c)) {
        case ColumnType::Number: numberCols << c; break;
        case ColumnType::Date: dateCols << c; break;
        case ColumnType::Text: textCols << c; break;
        }
    }

    // X: first date column, else first number column if there are two,
    // else the row number.
    if (!dateCols.isEmpty())
        m_xCol = dateCols.first();
    else if (numberCols.size() >= 2)
        m_xCol = numberCols.first();
    else
        m_xCol = -1;

    // Y: first number column that isn't X.
    m_yCol = -1;
    for (int c : std::as_const(numberCols)) {
        if (c != m_xCol) {
            m_yCol = c;
            break;
        }
    }

    // Filter: first text column with a manageable number of distinct values
    // (e.g. "kind"), so the chart shows one series instead of a mix.
    m_filterCol = -1;
    for (int c : std::as_const(textCols)) {
        QSet<QString> distinct;
        for (const QStringList &row : std::as_const(m_rows)) {
            distinct.insert(row.value(c).trimmed());
            if (distinct.size() > 200)
                break;
        }
        if (distinct.size() >= 2 && distinct.size() <= 200) {
            m_filterCol = c;
            break;
        }
    }
}

void GraphController::refreshFilterValues()
{
    m_filterValues.clear();
    if (m_filterCol < 0) {
        m_filterValue.clear();
        return;
    }

    QSet<QString> seen;
    for (const QStringList &row : std::as_const(m_rows)) {
        const QString v = row.value(m_filterCol).trimmed();
        if (!seen.contains(v)) {
            seen.insert(v);
            m_filterValues << v; // keep file order
            if (m_filterValues.size() >= kMaxFilterValues)
                break;
        }
    }
    if (!m_filterValues.contains(m_filterValue))
        m_filterValue = m_filterValues.value(0);
}

void GraphController::rebuildPoints()
{
    m_points.clear();
    m_xIsDate = m_xCol >= 0 && m_types.value(m_xCol) == ColumnType::Date;

    if (m_yCol < 0) {
        updateRange();
        setStatus(tr("%1: no number column to plot on Y").arg(m_fileName));
        emit dataLoaded();
        return;
    }

    int shownRows = 0;
    for (const QStringList &row : std::as_const(m_rows)) {
        if (m_filterCol >= 0 && row.value(m_filterCol).trimmed() != m_filterValue)
            continue;
        ++shownRows;

        double y = 0.0;
        if (!parseNumber(row.value(m_yCol), &y))
            continue;

        double x = 0.0;
        if (m_xCol < 0)
            x = shownRows;
        else if (m_xIsDate ? !parseDate(row.value(m_xCol), &x)
                           : !parseNumber(row.value(m_xCol), &x))
            continue;

        m_points.append(QPointF(x, y));
    }

    if (m_xCol >= 0) {
        std::stable_sort(m_points.begin(), m_points.end(),
                         [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    }

    updateRange();

    QString status = tr("%1 rows · %2 shown").arg(m_rows.size()).arg(m_points.size());
    if (m_skippedLines > 0)
        status += tr(" · %1 lines skipped").arg(m_skippedLines);
    setStatus(status);

    emit dataLoaded();
}

QVariantList GraphController::points() const
{
    QVariantList list;
    list.reserve(m_points.size());
    for (const QPointF &p : m_points)
        list.append(p); // arrives in QML as a 'point' with .x and .y
    return list;
}

void GraphController::clearGraph()
{
    m_headers.clear();
    m_rows.clear();
    m_types.clear();
    m_xCol = m_yCol = m_filterCol = -1;
    m_filterValue.clear();
    m_filterValues.clear();
    m_points.clear();
    m_fileName.clear();
    m_skippedLines = 0;
    updateRange();

    emit tableChanged();
    emit selectionChanged();
    emit graphCleared();
    emit dataLoaded();
    setStatus(QString());
}

// ---------------------------------------------------------------------------
// Column pickers
// ---------------------------------------------------------------------------

QStringList GraphController::xOptions() const
{
    QStringList options{kRowNumber};
    for (int c = 0; c < m_types.size(); ++c)
        if (m_types.at(c) != ColumnType::Text)
            options << m_headers.value(c);
    return options;
}

QStringList GraphController::yOptions() const
{
    QStringList options;
    for (int c = 0; c < m_types.size(); ++c)
        if (m_types.at(c) == ColumnType::Number)
            options << m_headers.value(c);
    return options;
}

QStringList GraphController::filterColumnOptions() const
{
    QStringList options{kNoFilter};
    for (int c = 0; c < m_types.size(); ++c)
        if (m_types.at(c) == ColumnType::Text)
            options << m_headers.value(c);
    return options;
}

QString GraphController::xColumnName() const
{
    return m_xCol < 0 ? kRowNumber : m_headers.value(m_xCol);
}

QString GraphController::yColumnName() const
{
    return m_yCol < 0 ? QString() : m_headers.value(m_yCol);
}

QString GraphController::filterColumnName() const
{
    return m_filterCol < 0 ? kNoFilter : m_headers.value(m_filterCol);
}

void GraphController::setXColumnName(const QString &name)
{
    const int col = (name == kRowNumber) ? -1 : static_cast<int>(m_headers.indexOf(name));
    if (col == m_xCol || (col < 0 && name != kRowNumber))
        return;
    m_xCol = col;
    emit selectionChanged();
    rebuildPoints();
}

void GraphController::setYColumnName(const QString &name)
{
    const int col = static_cast<int>(m_headers.indexOf(name));
    if (col < 0 || col == m_yCol)
        return;
    m_yCol = col;
    emit selectionChanged();
    rebuildPoints();
}

void GraphController::setFilterColumnName(const QString &name)
{
    const int col = (name == kNoFilter || name.isEmpty())
                        ? -1
                        : static_cast<int>(m_headers.indexOf(name));
    if (col == m_filterCol)
        return;
    m_filterCol = col;
    m_filterValue.clear();
    refreshFilterValues();
    emit selectionChanged();
    rebuildPoints();
}

void GraphController::setFilterValue(const QString &value)
{
    if (value == m_filterValue)
        return;
    m_filterValue = value;
    emit selectionChanged();
    rebuildPoints();
}

// ---------------------------------------------------------------------------

void GraphController::setStatus(const QString &message)
{
    if (m_statusMessage == message)
        return;
    m_statusMessage = message;
    emit statusChanged();
}

void GraphController::updateRange()
{
    if (m_points.isEmpty()) {
        m_minX = 0.0; m_maxX = 1.0;
        m_minY = 0.0; m_maxY = 1.0;
        return;
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = minX;
    double maxY = maxX;

    for (const QPointF &p : std::as_const(m_points)) {
        minX = qMin(minX, p.x()); maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y()); maxY = qMax(maxY, p.y());
    }

    // 5% padding; a flat range (all values equal) still gets a visible axis.
    const double spanX = maxX - minX;
    const double spanY = maxY - minY;
    const double padX = spanX > 0 ? spanX * 0.05 : (m_xIsDate ? 86400000.0 : 1.0);
    const double padY = spanY > 0 ? spanY * 0.05 : 1.0;

    m_minX = minX - padX; m_maxX = maxX + padX;
    m_minY = minY - padY; m_maxY = maxY + padY;
}
