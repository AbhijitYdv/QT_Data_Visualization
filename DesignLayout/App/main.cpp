
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include "graphcontroller.h"
#include "pluginmanager.h"

#include "autogen/environment.h"

int main(int argc, char *argv[])
{
    set_qt_environment();
    QApplication app(argc, argv);

    // Created before the engine so it is destroyed after it:
    // QML keeps reading from it until the engine has shut down.
    GraphController graphController;

    // Chart plugins: from the app's plugin folder (see datascope_add_plugin
    // in cmake/DataScopePlugin.cmake). A broken plugin is skipped with a
    // warning, never a crash.
    PluginManager pluginManager;
    pluginManager.loadStatic();
    pluginManager.loadFrom(PluginManager::defaultPluginDir());
    qInfo().noquote() << "Chart plugins loaded:" << pluginManager.count()
                      << "from" << PluginManager::defaultPluginDir();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(
        "graphController",
        &graphController
        );

    engine.rootContext()->setContextProperty(
        "pluginManager",
        &pluginManager
        );

    const QUrl url(mainQmlFile);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
        );

    engine.addImportPath(
        QCoreApplication::applicationDirPath() + "/qml"
        );
    engine.addImportPath(":/");

    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
