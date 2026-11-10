
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include "graphcontroller.h"
#include "usmap/usmapitem.h"

#include "autogen/environment.h"

int main(int argc, char *argv[])
{
    set_qt_environment();
    QApplication app(argc, argv);

    // The US map's drawing item, used from UsMap.qml as UsMapItem.
    qmlRegisterType<UsMapItem>("DataScope.Maps", 1, 0, "UsMapItem");

    // Created before the engine so it is destroyed after it:
    // QML keeps reading from it until the engine has shut down.
    GraphController graphController;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(
        "graphController",
        &graphController
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
