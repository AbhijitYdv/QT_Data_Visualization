// Tests for the US map projection, the bundled outlines and lat/lon detection.

#include "dataparser.h"
#include "usmapdata.h"

#include <QTest>

#include <numeric>

using namespace UsMap;

namespace {

constexpr int Text = static_cast<int>(CellType::Text);
constexpr int Number = static_cast<int>(CellType::Number);

QString stateOf(double lon, double lat)
{
    const Outline &outline = bundledOutline();
    const int i = outline.stateAt(project(lon, lat));
    return i < 0 ? QString() : outline.states.at(i).postal;
}

QVector<int> allRows(const GraphTable &t)
{
    QVector<int> rows(t.rows.size());
    std::iota(rows.begin(), rows.end(), 0);
    return rows;
}

GraphTable table(const QStringList &headers, const QList<QStringList> &rows, const QVector<int> &types)
{
    GraphTable t;
    t.headers = headers;
    t.rows = rows;
    t.types = types;
    return t;
}

} // namespace

class TestUsMap : public QObject
{
    Q_OBJECT

private slots:
    void projectionCentreAndDirections()
    {
        const QPointF centre = project(-96.0, 37.5);
        QVERIFY(qAbs(centre.x()) < 1e-12);
        QVERIFY(qAbs(centre.y()) < 1e-12);

        // East is +x, north is +y.
        QVERIFY(project(-75.0, 40.0).x() > project(-105.0, 40.0).x());
        QVERIFY(project(-96.0, 45.0).y() > project(-96.0, 30.0).y());
    }

    void outlineHasLower48AndDc()
    {
        const Outline &outline = bundledOutline();
        QCOMPARE(outline.states.size(), 49);
        QStringList postal;
        for (const State &s : outline.states)
            postal << s.postal;
        QVERIFY(postal.contains("NJ"));
        QVERIFY(postal.contains("DC"));
        QVERIFY(!postal.contains("AK"));
        QVERIFY(!postal.contains("HI"));
        QVERIFY(outline.bounds.width() > outline.bounds.height()); // wider than tall
    }

    void citiesLandInTheRightState()
    {
        QCOMPARE(stateOf(-75.1194, 39.7099), QStringLiteral("NJ")); // Rowan University
        QCOMPARE(stateOf(-76.8867, 40.2732), QStringLiteral("PA")); // Harrisburg
        QCOMPARE(stateOf(-89.6501, 39.7817), QStringLiteral("IL")); // Springfield
        QCOMPARE(stateOf(-104.9903, 39.7392), QStringLiteral("CO")); // Denver
        QCOMPARE(stateOf(-81.3792, 28.5383), QStringLiteral("FL")); // Orlando
        QCOMPARE(stateOf(-117.4260, 47.6588), QStringLiteral("WA")); // Spokane
        QCOMPARE(stateOf(-87.0, 43.0), QString());                    // Lake Michigan
        QCOMPARE(stateOf(-60.0, 40.0), QString());                    // Atlantic
    }

    void loadOutlineRejectsJunk()
    {
        QVERIFY(loadOutline("not json").states.isEmpty());
        QVERIFY(loadOutline(R"({"type":"FeatureCollection","features":[{"geometry":{"type":"Point","coordinates":[0,0]}}]})").states.isEmpty());
    }

    void detectsCommonColumnNames()
    {
        const GraphTable places = Parsers::parseDataFile(SAMPLES_DIR "/places.geojson");
        LatLonColumns c = detectLatLon(places);
        QVERIFY(c.isValid());
        QCOMPARE(places.headers.at(c.lat), QStringLiteral("latitude"));
        QCOMPARE(places.headers.at(c.lon), QStringLiteral("longitude"));

        const GraphTable nested = Parsers::parseDataFile(SAMPLES_DIR "/nested.json");
        c = detectLatLon(nested);
        QVERIFY(c.isValid());
        QCOMPARE(nested.headers.at(c.lat), QStringLiteral("station.lat"));
        QCOMPARE(nested.headers.at(c.lon), QStringLiteral("station.lon"));

        QVERIFY(!detectLatLon(Parsers::parseDataFile(SAMPLES_DIR "/basic.csv")).isValid());
    }

    void detectsMatchingPrefixAndChecksRange()
    {
        const GraphTable t = table({"end_lat", "start_lat", "end_lng", "start_lng", "lon"},
                                   {{"40", "39", "-75", "-74", "500"}},
                                   {Number, Number, Number, Number, Number});
        const LatLonColumns c = detectLatLon(t);
        QCOMPARE(t.headers.at(c.lat), QStringLiteral("end_lat"));
        QCOMPARE(t.headers.at(c.lon), QStringLiteral("end_lng")); // same prefix, not "lon" (out of range anyway)

        const GraphTable text = table({"lat", "lon"}, {{"a", "b"}}, {Text, Text});
        QVERIFY(!detectLatLon(text).isValid());
    }

    void extractCountsOutsideAndMissing()
    {
        const GraphTable t = table({"city", "lat", "lon"},
                                   {{"Glassboro", "39.70", "-75.11"},
                                    {"Honolulu", "21.31", "-157.86"},
                                    {"Nowhere", "", "-80"},
                                    {"Denver", "39.74", "-104.99"}},
                                   {Text, Number, Number});
        const Points p = extractPoints(t, allRows(t), {1, 2});
        QCOMPARE(p.lonLat.size(), 2);
        QCOMPARE(p.rows, (QVector<int>{0, 3}));
        QCOMPARE(p.outside, 1);
        QCOMPARE(p.missing, 1);
        QCOMPARE(p.lonLat.first(), QPointF(-75.11, 39.70));

        QVERIFY(extractPoints(t, allRows(t), {}).lonLat.isEmpty());
    }
};

QTEST_GUILESS_MAIN(TestUsMap)
#include "tst_usmap.moc"
