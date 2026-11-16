#include "ivisualizationplugin.h"

#include <QStandardItemModel>
#include <QtTest>

// A fake plugin, to prove the interface compiles, casts and works
// through qobject_cast the way PluginManager will use it.
class FakePlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_INTERFACES(IVisualizationPlugin)
public:
    PluginMetadata metadata() const override
    {
        return {"test.viz.fake", "Fake", "Test plugin", QUrl("qrc:/fake.svg"), "1.0"};
    }
    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() >= 2;
    }
    QUrl qmlSource() const override { return QUrl("qrc:/fake/Fake.qml"); }
    QObject *createBackend(QAbstractItemModel *, QObject *parent) override
    {
        return new QObject(parent);
    }
    QVariantMap defaultConfig() const override { return {{"bins", 10}}; }
};

class TestPluginApi : public QObject
{
    Q_OBJECT
private slots:
    void castsThroughQObject()
    {
        FakePlugin plugin;
        QObject *asObject = &plugin;
        auto *viz = qobject_cast<IVisualizationPlugin *>(asObject);
        QVERIFY(viz);
        QCOMPARE(viz->metadata().id, QString("test.viz.fake"));
    }
    void canVisualizeChecksModel()
    {
        FakePlugin plugin;
        QStandardItemModel oneCol(3, 1), twoCols(3, 2);
        QVERIFY(!plugin.canVisualize(nullptr));
        QVERIFY(!plugin.canVisualize(&oneCol));
        QVERIFY(plugin.canVisualize(&twoCols));
    }
    void backendIsOwnedByParent()
    {
        FakePlugin plugin;
        QObject parent;
        QObject *backend = plugin.createBackend(nullptr, &parent);
        QCOMPARE(backend->parent(), &parent);
    }
    void configIsFlat()
    {
        FakePlugin plugin;
        QCOMPARE(plugin.defaultConfig().value("bins").toInt(), 10);
    }
};

QTEST_GUILESS_MAIN(TestPluginApi)
#include "tst_pluginapi.moc"
