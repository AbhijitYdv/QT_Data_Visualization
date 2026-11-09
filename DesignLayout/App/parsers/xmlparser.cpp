// XML.
//
// Finds the element that repeats as a record and turns each one into a row:
//
//   <orders>
//     <order id="1"><date>2022-11-01</date><customer><city>Austin</city></customer></order>
//     <order id="2">...</order>
//   </orders>
//
//   -> columns id, date, customer.city
//
// How the record element is chosen (pass 1 over the file):
//   - count every element path that has attributes or child elements;
//   - take the most frequent one (the shallower one on a tie);
//   - if its parent repeats too, with at most 4 children per parent, move
//     up to the parent, so <book> wins over the two <author>s inside each
//     book while <row> still wins over a <group> holding 100 rows.
// If no structured element repeats, a repeating plain element becomes a
// single column, e.g. <values><v>1</v><v>2</v></values> -> column "v".
//
// Pass 2 reads the records. Attributes and leaf elements become columns;
// nested elements get dotted names; a field repeated inside one record is
// joined with "; ".

#include "formatparsers.h"

#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QXmlStreamReader>

namespace Parsers {

namespace {

QString tr(const char *text)
{
    return QCoreApplication::translate("Parsers", text);
}

QString parentOf(const QString &path)
{
    const qsizetype slash = path.lastIndexOf(u'/');
    return slash > 0 ? path.left(slash) : QString();
}

QString xmlError(const QXmlStreamReader &xml)
{
    return tr("Invalid XML at line %1: %2").arg(xml.lineNumber()).arg(xml.errorString());
}

// Pass 1: decides which element path is one record. Empty on failure.
QString findRecordPath(QIODevice *device, QString *error)
{
    struct Open { QString path; bool structured; };
    QList<Open> stack;
    QHash<QString, int> structuredCount;
    QHash<QString, int> leafCount;

    QXmlStreamReader xml(device);
    while (!xml.atEnd()) {
        switch (xml.readNext()) {
        case QXmlStreamReader::StartElement: {
            const QString parent = stack.isEmpty() ? QString() : stack.last().path;
            if (!stack.isEmpty())
                stack.last().structured = true;
            stack.append({parent + u'/' + xml.name().toString(), !xml.attributes().isEmpty()});
            break;
        }
        case QXmlStreamReader::EndElement: {
            const Open element = stack.takeLast();
            ++(element.structured ? structuredCount : leafCount)[element.path];
            break;
        }
        default:
            break;
        }
    }
    if (xml.hasError()) {
        *error = xmlError(xml);
        return {};
    }

    // Most frequent path; the shallower one on a tie.
    auto mostFrequent = [](const QHash<QString, int> &counts) {
        QString best;
        for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
            const int bestCount = counts.value(best);
            const bool better = it.value() > bestCount
                                || (it.value() == bestCount
                                    && it.key().count(u'/') < best.count(u'/'));
            if (best.isEmpty() || better)
                best = it.key();
        }
        return best;
    };

    // A structured element that repeats wins; then a plain one that repeats
    // (<v> in <values><v>1</v><v>2</v></values>); then a single structured one.
    QString best = mostFrequent(structuredCount);
    const QString bestLeaf = mostFrequent(leafCount);
    if (best.isEmpty() || (structuredCount.value(best) < 2 && leafCount.value(bestLeaf) >= 2))
        best = bestLeaf;
    if (best.isEmpty()) {
        *error = tr("No elements found");
        return {};
    }

    // Prefer an ancestor that repeats almost as often (<book> over its few
    // <author>s), but not a grouping element that holds many records each.
    for (QString up = parentOf(best); !up.isEmpty(); up = parentOf(up)) {
        const int upCount = structuredCount.value(up);
        const int bestCount = structuredCount.value(best, leafCount.value(best));
        if (upCount < 2 || upCount * 4 < bestCount)
            break;
        best = up;
    }
    return best;
}

// Collects one record's fields, joining repeats.
class RecordFields
{
public:
    void add(const QString &name, const QString &value)
    {
        const auto it = m_index.constFind(name);
        if (it == m_index.constEnd()) {
            m_index.insert(name, static_cast<int>(m_record.size()));
            m_record << qMakePair(name, value);
        } else if (!value.isEmpty()) {
            QString &existing = m_record[it.value()].second;
            existing = existing.isEmpty() ? value : existing + QStringLiteral("; ") + value;
        }
    }
    const Record &record() const { return m_record; }

private:
    Record m_record;
    QHash<QString, int> m_index;
};

} // namespace

GraphTable parseXml(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return errorTable(tr("Could not open the file"));

    QString error;
    const QString recordPath = findRecordPath(&file, &error);
    if (recordPath.isEmpty())
        return errorTable(error);
    const QString recordName = recordPath.mid(recordPath.lastIndexOf(u'/') + 1);

    // Pass 2: read the records.
    file.seek(0);
    QXmlStreamReader xml(&file);
    RecordTableBuilder builder;

    struct Open { QString name; QString text; bool hasChildren = false; };
    QStringList pathParts;         // full path of the current element
    QList<Open> inRecord;          // elements open inside the current record
    RecordFields fields;
    bool reading = false;

    auto fieldName = [&]() {       // dotted name below the record element
        QStringList parts;
        for (qsizetype i = 1; i < inRecord.size(); ++i)
            parts << inRecord.at(i).name;
        return parts.join(u'.');
    };

    while (!xml.atEnd()) {
        switch (xml.readNext()) {
        case QXmlStreamReader::StartElement: {
            const QString name = xml.name().toString();
            pathParts << name;
            if (!reading && u'/' + pathParts.join(u'/') == recordPath) {
                reading = true;
                fields = RecordFields();
            }
            if (!reading)
                break;
            if (!inRecord.isEmpty())
                inRecord.last().hasChildren = true;
            inRecord.append({name, QString(), false});
            const QString prefix = fieldName();
            for (const QXmlStreamAttribute &attribute : xml.attributes()) {
                const QString attr = attribute.name().toString();
                fields.add(prefix.isEmpty() ? attr : prefix + u'.' + attr,
                           attribute.value().toString().trimmed());
            }
            break;
        }
        case QXmlStreamReader::Characters:
            if (reading && !inRecord.isEmpty())
                inRecord.last().text += xml.text();
            break;
        case QXmlStreamReader::EndElement: {
            pathParts.removeLast();
            if (!reading)
                break;
            if (!inRecord.last().hasChildren) {
                const QString name = fieldName();
                const QString text = inRecord.last().text.trimmed();
                // A record with no children: its own text is the value.
                if (!name.isEmpty() || !text.isEmpty())
                    fields.add(name.isEmpty() ? recordName : name, text);
            }
            inRecord.removeLast();
            if (inRecord.isEmpty()) {
                builder.addRecord(fields.record());
                reading = false;
            }
            break;
        }
        default:
            break;
        }
    }
    if (xml.hasError())
        return errorTable(xmlError(xml));

    GraphTable table;
    builder.moveInto(table);
    finishTable(table);
    return table;
}

} // namespace Parsers
