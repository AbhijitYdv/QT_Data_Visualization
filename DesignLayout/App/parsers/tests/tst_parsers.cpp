// Tests for every data parser. Uses the files in samples/ plus small files
// written to a temporary folder for the edge cases.

#include "dataparser.h"

#include <QSqlDatabase>
#include <QTemporaryDir>
#include <QTest>

using Parsers::parseDataFile;

namespace {

constexpr int Text = static_cast<int>(CellType::Text);
constexpr int Number = static_cast<int>(CellType::Number);
constexpr int Date = static_cast<int>(CellType::Date);

QString sample(const char *name)
{
    return QStringLiteral(SAMPLES_DIR "/") + QLatin1String(name);
}

int typeOf(const GraphTable &table, const QString &column)
{
    const int c = static_cast<int>(table.headers.indexOf(column));
    return c < 0 ? -1 : table.types.at(c);
}

QString cell(const GraphTable &table, int row, const QString &column)
{
    return table.rows.value(row).value(static_cast<int>(table.headers.indexOf(column)));
}

} // namespace

class TestParsers : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_dir;

    QString writeFile(const QString &name, const QByteArray &content)
    {
        QFile file(m_dir.filePath(name));
        if (!file.open(QIODevice::WriteOnly))
            qFatal("cannot write test file");
        file.write(content);
        return file.fileName();
    }

    // Every successful table must be rectangular with one type per column.
    static void checkShape(const GraphTable &table)
    {
        QVERIFY2(table.error.isEmpty(), qPrintable(table.error));
        QVERIFY(!table.headers.isEmpty());
        QCOMPARE(table.types.size(), table.headers.size());
        for (const QStringList &row : table.rows)
            QCOMPARE(row.size(), table.headers.size());
    }

private slots:
    void initTestCase() { QVERIFY(m_dir.isValid()); }

    // ------------------------------------------------------------ delimited

    void csvStillWorks()
    {
        const GraphTable t = parseDataFile(sample("basic.csv"));
        checkShape(t);
        QCOMPARE(t.headers, (QStringList{"month", "sales"}));
        QCOMPARE(typeOf(t, "sales"), Number);
        QCOMPARE(t.fileName, QStringLiteral("basic.csv"));
    }

    void csvDatesAndSemicolons()
    {
        const GraphTable prices = parseDataFile(sample("prices.csv"));
        checkShape(prices);
        QCOMPARE(typeOf(prices, "date"), Date);
        QCOMPARE(typeOf(prices, "kind"), Text);

        const GraphTable semi = parseDataFile(sample("semicolon.csv"));
        checkShape(semi);
        QCOMPARE(semi.headers, (QStringList{"x", "y"}));
    }

    void tsvAndHeaderlessText()
    {
        const QString tsv = writeFile("t.tsv", "a\tb\n1\t2\n3\t4\n");
        QCOMPARE(parseDataFile(tsv).headers, (QStringList{"a", "b"}));

        const GraphTable plain = parseDataFile(sample("simple.txt"));
        checkShape(plain);
        QCOMPARE(plain.headers.first(), QStringLiteral("Column 1"));
    }

    // ----------------------------------------------------------------- JSON

    void jsonArrayOfObjects()
    {
        const GraphTable t = parseDataFile(sample("basic.json"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 12);
        QCOMPARE(typeOf(t, "date"), Date);
        QCOMPARE(typeOf(t, "units"), Number);
        QCOMPARE(typeOf(t, "region"), Text);
    }

    void jsonNestedRecords()
    {
        const GraphTable t = parseDataFile(sample("nested.json"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 21); // the "data" array, not "source"
        QVERIFY(t.headers.contains("station.lat"));
        QVERIFY(t.headers.contains("readings.temp"));
        QCOMPARE(typeOf(t, "readings.temp"), Number);
        QCOMPARE(cell(t, 0, "station.name"), QStringLiteral("Glassboro"));
        QCOMPARE(cell(t, 2, "tags"), QStringLiteral("[\"hourly\",\"raw\"]"));
    }

    void jsonColumns()
    {
        const GraphTable t = parseDataFile(sample("columns.json"));
        checkShape(t);
        QCOMPARE(t.headers, (QStringList{"x", "y"}));
        QCOMPARE(t.rows.size(), 20);
        QCOMPARE(cell(t, 1, "x"), QStringLiteral("0.5"));
    }

    void jsonColumnsOfDifferentLength()
    {
        const GraphTable t = parseDataFile(writeFile("c.json", R"({"a":[1,2,3],"b":[4]})"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 3);
        QCOMPARE(cell(t, 2, "b"), QString());
    }

    void jsonRowsWithHeader()
    {
        const GraphTable t = parseDataFile(
            writeFile("r.json", R"([["day","value"],["2026-01-01",3],["2026-01-02",5.5]])"));
        checkShape(t);
        QCOMPARE(t.headers, (QStringList{"day", "value"}));
        QCOMPARE(t.rows.size(), 2);
        QCOMPARE(typeOf(t, "day"), Date);
    }

    void jsonPlainValues()
    {
        const GraphTable t = parseDataFile(writeFile("v.json", "[1, 2.5, null, 4]"));
        checkShape(t);
        QCOMPARE(t.headers, QStringList{"value"});
        QCOMPARE(t.rows.size(), 4);
        QCOMPARE(t.rows.at(2).first(), QString());
    }

    void jsonValueFormatting()
    {
        const GraphTable t = parseDataFile(
            writeFile("f.json", R"([{"big":20221101,"small":0.1,"flag":true,"none":null}])"));
        checkShape(t);
        QCOMPARE(cell(t, 0, "big"), QStringLiteral("20221101"));
        QCOMPARE(cell(t, 0, "small"), QStringLiteral("0.1"));
        QCOMPARE(cell(t, 0, "flag"), QStringLiteral("true"));
        QCOMPARE(cell(t, 0, "none"), QString());
    }

    void jsonSingleObject()
    {
        const GraphTable t = parseDataFile(writeFile("o.json", R"({"a":1,"b":{"c":2}})"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 1);
        QCOMPARE(t.headers, (QStringList{"a", "b.c"}));
    }

    void jsonRecordsWithDifferentKeys()
    {
        const GraphTable t = parseDataFile(writeFile("k.json", R"([{"a":1},{"b":2},{"a":3,"b":4}])"));
        checkShape(t);
        QCOMPARE(t.headers, (QStringList{"a", "b"}));
        QCOMPARE(cell(t, 1, "a"), QString());
        QCOMPARE(cell(t, 1, "b"), QStringLiteral("2"));
    }

    void jsonErrors()
    {
        const GraphTable broken = parseDataFile(sample("broken.json"));
        QVERIFY(broken.error.startsWith("broken.json: Invalid JSON"));

        QVERIFY(!parseDataFile(writeFile("e.json", "[]")).error.isEmpty());
        QVERIFY(!parseDataFile(writeFile("e2.json", "{}")).error.isEmpty());
        QVERIFY(!parseDataFile(m_dir.filePath("missing.json")).error.isEmpty());
    }

    void jsonLines()
    {
        const GraphTable t = parseDataFile(sample("readings.jsonl"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 24);
        QCOMPARE(typeOf(t, "temp"), Number);
    }

    void jsonLinesSkipsBadLines()
    {
        const GraphTable t = parseDataFile(writeFile("b.ndjson", "{\"a\":1}\nnot json\n\n{\"a\":2}\n"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 2);
        QCOMPARE(t.skippedLines, 1);
    }

    void jsonLinesSavedAsJson()
    {
        const GraphTable t = parseDataFile(writeFile("l.json", "{\"a\":1}\n{\"a\":2}\n"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 2);
    }

    // -------------------------------------------------------------- GeoJSON

    void geoJson()
    {
        const GraphTable t = parseDataFile(sample("places.geojson"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 6);
        QCOMPARE(typeOf(t, "longitude"), Number);
        QCOMPARE(typeOf(t, "latitude"), Number);
        QCOMPARE(typeOf(t, "population"), Number);
        QCOMPARE(cell(t, 0, "name"), QStringLiteral("Rowan University"));
        QCOMPARE(cell(t, 0, "geometry"), QStringLiteral("Point"));
        QCOMPARE(cell(t, 0, "latitude").toDouble(), 39.7099);

        // The polygon gets the average of its corners (first corner repeated).
        QCOMPARE(cell(t, 5, "geometry"), QStringLiteral("Polygon"));
        QVERIFY(qAbs(cell(t, 5, "latitude").toDouble() - 39.76) < 1e-9);
    }

    void geoJsonInsideJsonFile()
    {
        const GraphTable t = parseDataFile(
            writeFile("g.json", R"({"type":"Point","coordinates":[-75.1,39.7,12]})"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 1);
        QCOMPARE(cell(t, 0, "altitude"), QStringLiteral("12"));
    }

    void geoJsonRejectsPlainJson()
    {
        const GraphTable t = parseDataFile(writeFile("p.geojson", R"([{"a":1}])"));
        QVERIFY(t.error.contains("GeoJSON"));
    }

    // --------------------------------------------------------------- SQLite

    void sqlitePicksLargestTable()
    {
        const GraphTable t = parseDataFile(sample("shop.db"));
        checkShape(t);
        QCOMPARE(t.section, QStringLiteral("orders"));
        QCOMPARE(t.fileName, QStringLiteral("shop.db – orders"));
        QCOMPARE(t.rows.size(), 20);
        QCOMPARE(typeOf(t, "order_date"), Date);
        QCOMPARE(typeOf(t, "total"), Number);
        QCOMPARE(cell(t, 1, "note"), QString()); // NULL -> ""
    }

    void sqliteLeavesNoConnectionsOpen()
    {
        parseDataFile(sample("shop.db"));
        QVERIFY(QSqlDatabase::connectionNames().isEmpty());
    }

    void sqliteErrors()
    {
        const GraphTable notDb = parseDataFile(writeFile("fake.db", "just some text\n"));
        QVERIFY(!notDb.error.isEmpty());

        const GraphTable empty = parseDataFile(writeFile("empty.sqlite", ""));
        QVERIFY(!empty.error.isEmpty());
    }

    // ------------------------------------------------------------------ XML

    void xmlRecords()
    {
        const GraphTable t = parseDataFile(sample("inventory.xml"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 5);
        QCOMPARE(t.headers,
                 (QStringList{"sku", "name", "qty", "price.currency", "price", "received"}));
        QCOMPARE(typeOf(t, "qty"), Number);
        QCOMPARE(typeOf(t, "received"), Date);
        QCOMPARE(cell(t, 4, "sku"), QStringLiteral("D777"));
    }

    void xmlPrefersRecordOverRepeatedChild()
    {
        const GraphTable t = parseDataFile(writeFile("books.xml", R"(
            <catalog>
              <book><title>A</title><author>X</author><author>Y</author></book>
              <book><title>B</title><author>Z</author><author>W</author></book>
            </catalog>)"));
        checkShape(t);
        QCOMPARE(t.rows.size(), 2);
        QCOMPARE(cell(t, 0, "author"), QStringLiteral("X; Y"));
    }

    void xmlSkipsGroupingElements()
    {
        QByteArray xml = "<data>";
        for (int g = 0; g < 2; ++g) {
            xml += "<group>";
            for (int r = 0; r < 10; ++r)
                xml += "<row a=\"" + QByteArray::number(r) + "\"><b>1</b></row>";
            xml += "</group>";
        }
        xml += "</data>";
        const GraphTable t = parseDataFile(writeFile("groups.xml", xml));
        checkShape(t);
        QCOMPARE(t.rows.size(), 20);
        QCOMPARE(t.headers, (QStringList{"a", "b"}));
    }

    void xmlPlainRepeatedValues()
    {
        const GraphTable t = parseDataFile(writeFile("v.xml", "<values><v>1</v><v>2</v><v>3</v></values>"));
        checkShape(t);
        QCOMPARE(t.headers, QStringList{"v"});
        QCOMPARE(t.rows.size(), 3);
    }

    void xmlErrors()
    {
        const GraphTable t = parseDataFile(writeFile("bad.xml", "<a><b></a>"));
        QVERIFY(t.error.contains("Invalid XML"));
    }

    // ------------------------------------------------------------- dispatch

    void unknownExtensionIsSniffed()
    {
        QCOMPARE(parseDataFile(writeFile("j.dat", R"([{"a":1}])")).headers, QStringList{"a"});
        QCOMPARE(parseDataFile(writeFile("x.dat", "<r><i a=\"1\"/><i a=\"2\"/></r>")).rows.size(), 2);
        QCOMPARE(parseDataFile(writeFile("c.dat", "a,b\n1,2\n")).headers, (QStringList{"a", "b"}));

        QFile::copy(sample("shop.db"), m_dir.filePath("shop.bin"));
        QCOMPARE(parseDataFile(m_dir.filePath("shop.bin")).section, QStringLiteral("orders"));
    }

    void extensionIsCaseInsensitive()
    {
        QCOMPARE(parseDataFile(writeFile("UP.JSON", R"([{"a":1}])")).headers, QStringList{"a"});
    }

    void fileDialogFilters()
    {
        const QStringList filters = Parsers::fileDialogFilters();
        QVERIFY(filters.first().contains("*.json"));
        QVERIFY(filters.first().contains("*.sqlite"));
        QCOMPARE(filters.last(), QStringLiteral("All files (*)"));
    }
};

QTEST_GUILESS_MAIN(TestParsers)
#include "tst_parsers.moc"
