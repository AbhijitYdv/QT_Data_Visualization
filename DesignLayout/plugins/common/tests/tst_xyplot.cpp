#include "datasettablemodel.h"
#include "ivisualizationplugin.h"
#include "pluginmanager.h"
#include "seriesbackend.h"

#include <QLineSeries>
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
        t.rows << QStringList{QString("r%1").arg(i),
                              QDate(2024, 1, 1).addDays(i).toString(Qt::ISODate),
                              QString::number(rows - i), QString::number(i % 7)};
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
} // namespace

class TestXYPlot : public QObject
{
    Q_OBJECT

    PluginManager pm;

    QObject *backendFor(const QString &id, QAbstractItemModel *model, QObject *parent,
                        const QString &x, const QString &y)
    {
        QObject *b = pm.createBackend(id, model, parent);
        if (!b)
            return nullptr;
        b->setProperty("xColumn", x);
        b->setProperty("yColumn", y);
        QMetaObject::invokeMethod(b, "refresh");
        return b;
    }

private slots:
    void initTestCase()
    {
        pm.loadFrom(LINE_PLUGIN_DIR);
        pm.loadFrom(AREA_PLUGIN_DIR);
        pm.loadFrom(SCATTER_PLUGIN_DIR);
        QVERIFY2(pm.has("core.viz.line") && pm.has("core.viz.area") && pm.has("core.viz.scatter"),
                 qPrintable(pm.errors().join('\n')));
    }

    void viewsAreEmbedded()
    {
        for (const char *path : {":/plugins/linechart_plugin/LineChart.qml",
                                 ":/plugins/areachart_plugin/AreaChart.qml",
                                 ":/plugins/scatterplot_plugin/ScatterPlot.qml",
                                 ":/plugins/linechart_plugin/icon.svg"})
            QVERIFY2(QFile::exists(path), path);
        QCOMPARE(pm.qmlSource("core.viz.scatter"),
                 QUrl("qrc:/plugins/scatterplot_plugin/ScatterPlot.qml"));
    }

    void thinKeepsPeaks()
    {
        QList<QPointF> pts;
        for (int i = 0; i < 10000; ++i)
            pts << QPointF(i, i == 4321 ? 999 : (i == 777 ? -999 : 0));
        const QList<QPointF> t = SeriesBackend::thin(pts, 2000);
        QVERIFY(t.size() <= 2000);
        bool hi = false, lo = false;
        for (const QPointF &p : t) {
            hi |= p.y() == 999;
            lo |= p.y() == -999;
        }
        QVERIFY(hi && lo);
        QCOMPARE(SeriesBackend::thin(pts.mid(0, 50), 2000).size(), 50); // small: untouched
    }

    void sampleIsEven()
    {
        QList<QPointF> pts;
        for (int i = 0; i < 100; ++i)
            pts << QPointF(i, i);
        const QList<QPointF> s = SeriesBackend::sample(pts, 10);
        QCOMPARE(s.size(), 10);
        QCOMPARE(s.at(1).x(), 10.0);
        QCOMPARE(s.last().x(), 90.0);
    }

    void lineSortsAndPads()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(5); // x = 5,4,3,2,1  y = 0,1,2,3,4
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = backendFor("core.viz.line", &model, &parent, "x", "y");
        QVERIFY(b);
        QCOMPARE(b->property("pointCount").toInt(), 5);
        QCOMPARE(b->property("plottedCount").toInt(), 5);
        QCOMPARE(b->property("minX").toDouble(), 1.0 - 0.2); // 5% of span 4
        QCOMPARE(b->property("maxX").toDouble(), 5.0 + 0.2);
        QCOMPARE(b->property("dataMinY").toDouble(), 0.0);
        QCOMPARE(b->property("dataMaxY").toDouble(), 4.0);

        QLineSeries series;
        QMetaObject::invokeMethod(b, "fillSeries", Q_ARG(QObject *, &series));
        QCOMPARE(series.count(), 5);
        QCOMPARE(series.at(0), QPointF(1, 4)); // sorted by X
        QCOMPARE(series.at(4), QPointF(5, 0));
    }

    void largeDataLineThinsScatterSamples()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(20000);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *line = backendFor("core.viz.line", &model, &parent, "Row #", "y");
        QObject *scatter = backendFor("core.viz.scatter", &model, &parent, "Row #", "y");
        QCOMPARE(line->property("pointCount").toInt(), 20000);
        QVERIFY(line->property("plottedCount").toInt() <= 2000);
        QCOMPARE(scatter->property("plottedCount").toInt(), 5000);
    }

    void dateAxisAndDayPadding()
    {
        DatasetTableModel model;
        GraphTable t = makeTable(1);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = backendFor("core.viz.area", &model, &parent, "day", "y");
        QVERIFY(b->property("xIsDate").toBool());
        // One point: X padded by one day each side.
        QCOMPARE(b->property("maxX").toDouble() - b->property("minX").toDouble(),
                 2 * 86400000.0);
    }

    void followsModelAndColumns()
    {
        DatasetTableModel model;
        const GraphTable t = makeTable(10);
        model.setSource(t, allRows(t));
        QObject parent;
        QObject *b = backendFor("core.viz.line", &model, &parent, "Row #", "y");
        QCOMPARE(b->property("pointCount").toInt(), 10);

        QSignalSpy changed(b, SIGNAL(changed()));
        model.setSource(t, {0, 1, 2}); // filter
        QVERIFY(changed.wait(1000));
        QCOMPARE(b->property("pointCount").toInt(), 3);

        b->setProperty("yColumn", "label"); // text: nothing to plot
        QVERIFY(changed.wait(1000));
        QCOMPARE(b->property("pointCount").toInt(), 0);
    }

    void fillSeriesIgnoresNonSeries()
    {
        DatasetTableModel model;
        QObject parent;
        QObject *b = pm.createBackend("core.viz.line", &model, &parent);
        QObject notASeries;
        QMetaObject::invokeMethod(b, "fillSeries", Q_ARG(QObject *, &notASeries));
        QMetaObject::invokeMethod(b, "fillSeries", Q_ARG(QObject *, nullptr));
    }
};

QTEST_MAIN(TestXYPlot)
#include "tst_xyplot.moc"
