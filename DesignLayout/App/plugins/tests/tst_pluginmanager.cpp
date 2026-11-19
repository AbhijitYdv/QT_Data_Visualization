#include "ivisualizationplugin.h"
#include "pluginmanager.h"

#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTemporaryDir>
#include <QtTest>

class TestPluginManager : public QObject
{
    Q_OBJECT
private slots:
    void loadsGoodSkipsBad()
    {
        PluginManager pm;
        QSignalSpy loaded(&pm, &PluginManager::pluginLoaded);
        QSignalSpy failed(&pm, &PluginManager::pluginFailed);

        pm.loadFrom(FIXTURE_DIR);

        // dup_plugin and good_plugin share an id: whichever loads first
        // (alphabetical: dup) wins, the other is rejected as a duplicate.
        QCOMPARE(pm.count(), 1);
        QCOMPARE(loaded.count(), 1);
        QVERIFY(pm.has("test.viz.good"));
        QCOMPARE(failed.count(), 2); // duplicate id + wrong interface

        const QString errors = pm.errors().join('\n');
        QVERIFY2(errors.contains("Duplicate plugin id"), qPrintable(errors));
        QVERIFY2(errors.contains("Wrong interface"), qPrintable(errors));
    }

    void lookupsWork()
    {
        PluginManager pm;
        pm.loadFrom(FIXTURE_DIR);
        IVisualizationPlugin *viz = pm.visualization("test.viz.good");
        QVERIFY(viz);
        QCOMPARE(pm.defaultConfig("test.viz.good").value("bins").toInt(), 12);
        QVERIFY(pm.qmlSource("test.viz.good").isValid());
        QVERIFY(!pm.visualization("nope"));
        QVERIFY(pm.qmlSource("nope").isEmpty());

        QObject parent;
        QObject *backend = viz->createBackend(nullptr, &parent);
        QCOMPARE(backend->parent(), &parent);
    }

    void createBackendForQml()
    {
        PluginManager pm;
        pm.loadFrom(FIXTURE_DIR);
        QStandardItemModel model(2, 2);
        QObject parent;
        QObject *backend = pm.createBackend("test.viz.good", &model, &parent);
        QVERIFY(backend);
        QCOMPARE(backend->parent(), &parent);
        QVERIFY(!pm.createBackend("nope", &model, &parent));
        QVERIFY(!pm.createBackend("test.viz.good", &parent, &parent)); // not a model
    }

    void qmlListHasFields()
    {
        PluginManager pm;
        pm.loadFrom(FIXTURE_DIR);
        const QVariantMap first = pm.pluginList().value(0).toMap();
        QCOMPARE(first.value("id").toString(), QString("test.viz.good"));
        QVERIFY(first.contains("name"));
        QVERIFY(first.contains("icon"));
    }

    void loadingTwiceIsSafe()
    {
        PluginManager pm;
        pm.loadFrom(FIXTURE_DIR);
        QSignalSpy changed(&pm, &PluginManager::pluginsChanged);
        QSignalSpy failed(&pm, &PluginManager::pluginFailed);
        pm.loadFrom(FIXTURE_DIR);
        QCOMPARE(pm.count(), 1);
        QCOMPARE(changed.count(), 0);
        QCOMPARE(failed.count(), 0);
    }

    void junkFilesNeverCrash()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        // A "library" that is really just text, plus a plain text file.
        for (const char *name : {"libjunk.so", "junk.dll", "libjunk.dylib", "notes.txt"}) {
            QFile f(QDir(tmp.path()).filePath(name));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("not a plugin");
        }
        PluginManager pm;
        pm.loadFrom(tmp.path());
        QCOMPARE(pm.count(), 0);
    }

    void missingFolderReportsFailure()
    {
        PluginManager pm;
        QSignalSpy failed(&pm, &PluginManager::pluginFailed);
        pm.loadFrom("/definitely/not/here");
        QCOMPARE(failed.count(), 1);
    }

    void defaultDirIsSet()
    {
        QVERIFY(!PluginManager::defaultPluginDir().isEmpty());
    }
};

QTEST_GUILESS_MAIN(TestPluginManager)
#include "tst_pluginmanager.moc"
