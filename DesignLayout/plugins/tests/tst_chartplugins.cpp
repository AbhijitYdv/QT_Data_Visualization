#include "datasettablemodel.h"
#include "ivisualizationplugin.h"
#include "pluginmanager.h"

#include <QLineSeries>
#include <QtTest>

namespace {
// city (text), region (text), lat, lon, pop (numbers)
GraphTable cities()
{
    GraphTable t;
    t.headers = {"city", "region", "latitude", "longitude", "pop"};
    const int txt = int(CellType::Text), num = int(CellType::Number);
    t.types = {txt, txt, num, num, num};
    t.rows = {
        {"Newark", "Northeast", "40.73", "-74.17", "300"},
        {"Boston", "Northeast", "42.36", "-71.06", "650"},
        {"Austin", "South", "30.27", "-97.74", "950"},
        {"Miami", "South", "25.76", "-80.19", "450"},
        {"Tampa", "South", "27.95", "-82.46", "n/a"},
        {"Denver", "West", "39.74", "-104.99", "700"},
        {"Honolulu", "West", "21.31", "-157.86", "350"}, // outside the lower 48
        {"Nowhere", "", "", "", "10"},                    // no coordinates, blank region
    };
    return t;
}

QVector<int> rowsWhere(const GraphTable &t, int col, const QString &value)
{
    QVector<int> r;
    for (int i = 0; i < t.rows.size(); ++i)
        if (col < 0 || t.rows.at(i).at(col) == value)
            r << i;
    return r;
}

void settle(QObject *backend)
{
    QMetaObject::invokeMethod(backend, "refresh");
}
} // namespace

class TestChartPlugins : public QObject
{
    Q_OBJECT
    PluginManager pm;

private slots:
    void initTestCase()
    {
        pm.loadFrom(HISTOGRAM_PLUGIN_DIR);
        pm.loadFrom(PIE_PLUGIN_DIR);
        pm.loadFrom(MAP_PLUGIN_DIR);
        QVERIFY2(pm.has("core.viz.histogram") && pm.has("core.viz.pie")
                     && pm.has("core.viz.usmap"),
                 qPrintable(pm.errors().join('\n')));
        for (const char *path : {":/plugins/histogram_plugin/Histogram.qml",
                                 ":/plugins/piechart_plugin/PieChart.qml",
                                 ":/plugins/piechart_plugin/ColumnPicker.qml",
                                 ":/plugins/usmap_plugin/UsMap.qml"})
            QVERIFY2(QFile::exists(path), path);
    }

    // ----------------------------------------------------------- histogram

    void histogramCountsYValues()
    {
        const GraphTable t = cities();
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.histogram", &model, &parent);
        b->setProperty("yColumn", "pop");
        settle(b);

        // 7 numbers (n/a skipped): Sturges -> 1 + log2(7) = 3.8 -> 5 (minimum)
        QCOMPARE(b->property("autoBins").toInt(), 5);
        QCOMPARE(b->property("binCount").toInt(), 5);
        int total = 0;
        for (const QVariant &c : b->property("counts").toList())
            total += c.toInt();
        QCOMPARE(total, 7);
        QCOMPARE(b->property("edges").toList().size(), 6);
        QCOMPARE(b->property("labels").toStringList().size(), 5);

        b->setProperty("bins", 3);
        settle(b);
        QCOMPARE(b->property("binCount").toInt(), 3);

        QLineSeries outline;
        QMetaObject::invokeMethod(b, "fillSeries", Q_ARG(QObject *, &outline));
        QCOMPARE(outline.count(), 3 * 4);

        // New dataset -> back to automatic bins
        GraphTable other = t;
        other.headers[4] = "population";
        model.setSource(other, rowsWhere(other, -1, {}));
        settle(b);
        QCOMPARE(b->property("bins").toInt(), 0);
    }

    // ------------------------------------------------------------------ pie

    void pieStartsOnFirstTextColumnAndCounts()
    {
        const GraphTable t = cities();
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.pie", &model, &parent);
        settle(b);
        QCOMPARE(b->property("groupOptions").toStringList(), (QStringList{"city", "region"}));
        QCOMPARE(b->property("groupColumn").toString(), QString("city"));

        b->setProperty("groupColumn", "region");
        settle(b);
        QCOMPARE(b->property("labels").toStringList(),
                 (QStringList{"South", "Northeast", "West", "(blank)"}));
        QCOMPARE(b->property("total").toDouble(), 8.0);
        QCOMPARE(b->property("valueName").toString(), QString("Rows"));
        QCOMPARE(b->property("highlight").toInt(), -1);
    }

    void pieSumsY()
    {
        const GraphTable t = cities();
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.pie", &model, &parent);
        b->setProperty("groupColumn", "region");
        b->setProperty("yColumn", "pop");
        b->setProperty("sumsY", true);
        settle(b);
        // South 950+450 (Tampa n/a), West 700+350, Northeast 300+650, blank 10
        QCOMPARE(b->property("labels").toStringList(),
                 (QStringList{"South", "West", "Northeast", "(blank)"}));
        QCOMPARE(b->property("values").toList().first().toDouble(), 1400.0);
        QCOMPARE(b->property("valueName").toString(), QString("Sum of pop"));
    }

    void pieByFilterColumnShowsAllGroups()
    {
        const GraphTable t = cities();
        DatasetTableModel filtered, all;
        filtered.setSource(t, rowsWhere(t, 1, "Northeast"));
        all.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.pie", &filtered, &parent);
        b->setProperty("allRowsModel", QVariant::fromValue<QAbstractItemModel *>(&all));
        b->setProperty("filterColumn", "region");
        b->setProperty("filterValue", "Northeast");
        settle(b);
        // Starts on the filter column; every region, Northeast pulled out.
        QCOMPARE(b->property("groupColumn").toString(), QString("region"));
        QCOMPARE(b->property("labels").toStringList().size(), 4);
        const int h = b->property("highlight").toInt();
        QCOMPARE(b->property("labels").toStringList().value(h), QString("Northeast"));
        QCOMPARE(b->property("rowCount").toInt(), 2); // the filtered rows
    }

    void pieOtherSlice()
    {
        GraphTable t;
        t.headers = {"k"};
        t.types = {int(CellType::Text)};
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j <= i; ++j)
                t.rows << QStringList{QString("g%1").arg(i)};
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.pie", &model, &parent);
        settle(b);
        QCOMPARE(b->property("labels").toStringList().size(), 8); // 7 + Other
        QVERIFY(b->property("hasOther").toBool());
        QCOMPARE(b->property("labels").toStringList().last(), QString("Other"));
    }

    // ------------------------------------------------------------------ map

    void mapFindsColumnsAndCounts()
    {
        const GraphTable t = cities();
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, -1, {}));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.usmap", &model, &parent);
        settle(b);
        QCOMPARE(b->property("latColumn").toString(), QString("latitude"));
        QCOMPARE(b->property("lonColumn").toString(), QString("longitude"));
        QVERIFY(b->property("hasCoordinates").toBool());
        QCOMPARE(b->property("pointCount").toInt(), 6);
        QCOMPARE(b->property("outside").toInt(), 1);  // Honolulu
        QCOMPARE(b->property("missing").toInt(), 1);  // Nowhere

        QString summary;
        QMetaObject::invokeMethod(b, "rowSummary", Q_RETURN_ARG(QString, summary), Q_ARG(int, 0));
        QCOMPARE(summary, QString("city: Newark\nregion: Northeast\npop: 300"));

        // Swapping the columns by hand: most points fall outside.
        b->setProperty("latColumn", "longitude");
        b->setProperty("lonColumn", "latitude");
        settle(b);
        QCOMPARE(b->property("latColumn").toString(), QString("longitude"));
        QVERIFY(b->property("pointCount").toInt() < 6);
        b->setProperty("latColumn", "city"); // not a number column: ignored
        settle(b);
        QCOMPARE(b->property("latColumn").toString(), QString("longitude"));
    }

    void mapFollowsFilterAndNoCoordinates()
    {
        const GraphTable t = cities();
        DatasetTableModel model;
        model.setSource(t, rowsWhere(t, 1, "South"));
        QObject parent;
        QObject *b = pm.createBackend("core.viz.usmap", &model, &parent);
        settle(b);
        QCOMPARE(b->property("pointCount").toInt(), 3);

        GraphTable plain;
        plain.headers = {"a", "b"};
        plain.types = {int(CellType::Number), int(CellType::Number)};
        plain.rows = {{"1", "2"}};
        model.setSource(plain, {0});
        settle(b);
        QVERIFY(!b->property("hasCoordinates").toBool());
        QCOMPARE(b->property("pointCount").toInt(), 0);
    }
};

QTEST_MAIN(TestChartPlugins)
#include "tst_chartplugins.moc"
