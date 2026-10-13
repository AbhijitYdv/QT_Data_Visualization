
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "graphcontroller.h"

#include "autogen/environment.h"

int main(int argc, char *argv[])
{
    set_qt_environment();
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    GraphController graphController;

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
