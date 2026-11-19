#include "datasettablemodel.h"
#include "ivisualizationplugin.h"
#include "pluginmanager.h"

#include <QSignalSpy>
#include <QtTest>

namespace {
// cols: label (text), day (date), x (number), y (number)
GraphTable makeTable(int rows)
{
    GraphTable t;
    t.headers = {"label", "day", "x", "y"};
    t.types = {int(CellType::Text), int(CellType::Date), int(CellType::Number),
               int(CellType::Number)};
    for (int i = 0; i < rows; ++i) {
        // x counts down so sorting by X reverses file order
        t.rows << QStringList{QString("r%1").arg(i),
                              QDate(2024, 1, 1).addDays(i).toString(Qt::ISODate),
                              QString::number(rows - i), QString::number(i * 10)};
    }
    return t;
}

QVector<int> allRows(const GraphTable &t)
{
    QVector<int> r;
    for (int i = 0; i < t.rows.size(); ++i)
        r << i;
    return r;
}

QList<double> values(QObject *b)
{
    QList<double> out;
    for (const QVariant &v : b->property("values").toList())
        out << v.toDouble();
    return out;
}
} // namespace

class TestBarChart : public QObject
{
    Q_OBJECT

    PluginManager pm;
    IVisualizationPlugin *viz = nullptr;

private slots:
    void initTestCase()
    {
        pm.loadFrom(BAR_PLUGIN_DIR);
        viz = pm.visualization("core.viz.bar");
        QVERIFY2(viz, qPrintable(pm.errors().join('\n')));
    }

    void metadataAndView()
    {
        QCOMPARE(viz->metadata().displayName, QString("Bar chart"));
        QCOMPARE(viz->qmlSource(), QUrl("qrc:/plugins/barchart_plugin/BarChart.qml"));
        QVERIFY(QFile::exists(":/plugins/barchart_plugin/BarChart.qml"));
        QVERIFY(QFile::exists(":/plugins/barchart_plugin/icon.svg"));
        QCOMPARE(viz->defaultConfig().value("maxBars").toInt(), 50);
    }

    void oneBarPerPointSortedByX()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(5);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("xColumn", "x");
        b->setProperty("yColumn", "y");
        QMetaObject::invokeMethod(b, "refresh");

        QCOMPARE(b->property("labels").toStringList(), (QStringList{"1", "2", "3", "4", "5"}));
        QCOMPARE(values(b), (QList<double>{40, 30, 20, 10, 0}));
        QCOMPARE(b->property("grouped").toBool(), false);
        QCOMPARE(b->property("dataMinY").toDouble(), 0.0);
        QCOMPARE(b->property("dataMaxY").toDouble(), 40.0);
        QCOMPARE(b->property("minY").toDouble(), -2.0); // padded 5% for the axis
        QCOMPARE(b->property("maxY").toDouble(), 42.0);
    }

    void rowNumberXIgnoresTableSort()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(4);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("xColumn", "Row #"); // not a column -> row number
        b->setProperty("yColumn", "y");
        QMetaObject::invokeMethod(b, "refresh");
        const QList<double> before = values(b);
        QCOMPARE(before, (QList<double>{0, 10, 20, 30}));

        model.sortByColumn(3); // ascending
        model.sortByColumn(3); // descending
        QMetaObject::invokeMethod(b, "refresh");
        QCOMPARE(values(b), before);
        QCOMPARE(b->property("labels").toStringList(), (QStringList{"1", "2", "3", "4"}));
    }

    void dateLabels()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(3);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("xColumn", "day");
        b->setProperty("yColumn", "y");
        QMetaObject::invokeMethod(b, "refresh");
        QVERIFY(b->property("xIsDate").toBool());
        QCOMPARE(b->property("labels").toStringList(), (QStringList{"01-01", "01-02", "01-03"}));
    }

    void largeDataIsGroupedAndAveraged()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(100);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("xColumn", "Row #");
        b->setProperty("yColumn", "y");
        QMetaObject::invokeMethod(b, "refresh");
        QVERIFY(b->property("grouped").toBool());
        const QList<double> v = values(b);
        QCOMPARE(v.size(), 50);
        QCOMPARE(v.first(), 5.0);   // avg(0, 10)
        QCOMPARE(v.last(), 985.0);  // avg(980, 990)
        QCOMPARE(b->property("pointCount").toInt(), 100);
    }

    void skipsBadValuesAndFollowsFilter()
    {
        DatasetTableModel model;
        GraphTable t = makeTable(4);
        t.rows[1][3] = "n/a";
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("xColumn", "Row #");
        b->setProperty("yColumn", "y");
        QMetaObject::invokeMethod(b, "refresh");
        // Row 2 (n/a) skipped, but still counted for row numbers.
        QCOMPARE(b->property("labels").toStringList(), (QStringList{"1", "3", "4"}));

        // Filter change in the app = setSource with fewer rows -> auto refresh.
        QSignalSpy changed(b, SIGNAL(changed()));
        model.setSource(t, {0, 3});
        QVERIFY(changed.wait(1000));
        QCOMPARE(values(b), (QList<double>{0, 30}));
    }

    void textOrMissingYGivesNoBars()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(3);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = viz->createBackend(&model, &parent);
        b->setProperty("yColumn", "label");
        QMetaObject::invokeMethod(b, "refresh");
        QCOMPARE(b->property("pointCount").toInt(), 0);
        b->setProperty("yColumn", "nope");
        QMetaObject::invokeMethod(b, "refresh");
        QCOMPARE(b->property("pointCount").toInt(), 0);

        QObject *noModel = viz->createBackend(nullptr, &parent);
        QCOMPARE(noModel->property("pointCount").toInt(), 0);
    }
};

QTEST_GUILESS_MAIN(TestBarChart)
#include "tst_barchart.moc"
