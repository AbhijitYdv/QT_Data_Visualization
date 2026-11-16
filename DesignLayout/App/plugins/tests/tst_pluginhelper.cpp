#include "ivisualizationplugin.h"
#include "pluginmanager.h"

#include <QDir>
#include <QFile>
#include <QtTest>

// Checks the whole build chain from step 3: datascope_add_plugin() built
// demo_plugin, copied it into this executable's plugin folder, and
// embedded its QML + icon in the plugin's own resources.
class TestPluginHelper : public QObject
{
    Q_OBJECT
private slots:
    void pluginIsInDefaultFolder()
    {
        PluginManager pm;
        pm.loadFrom(PluginManager::defaultPluginDir());
        QVERIFY2(pm.has("test.viz.demo"),
                 qPrintable("Not found in " + PluginManager::defaultPluginDir()
                            + "\n" + pm.errors().join('\n')));
    }

    void qmlAndIconAreEmbedded()
    {
        PluginManager pm;
        pm.loadFrom(PluginManager::defaultPluginDir());
        const QUrl qml = pm.qmlSource("test.viz.demo");
        QCOMPARE(qml, QUrl("qrc:/plugins/demo_plugin/Demo.qml"));
        // Resources register when the library loads.
        QVERIFY(QFile::exists(":/plugins/demo_plugin/Demo.qml"));
        QVERIFY(QFile::exists(":/plugins/demo_plugin/icon.svg"));
    }
};

QTEST_GUILESS_MAIN(TestPluginHelper)
#include "tst_pluginhelper.moc"
