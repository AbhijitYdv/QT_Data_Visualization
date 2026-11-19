#include "ivisualizationplugin.h"
#include "mapbackend.h"
#include "usmapview.h"

#include <QQmlEngine>

// The US map as a runtime plugin. View and icon are embedded under
// qrc:/plugins/usmap_plugin/; the QPainter map item is registered for QML
// as DataScope.MapPlugin / UsMapView when the plugin is loaded.
class UsMapPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    UsMapPlugin()
    {
        // Before any view is loaded: PluginManager creates this object when
        // it loads the library.
        qmlRegisterType<UsMapView>("DataScope.MapPlugin", 1, 0, "UsMapView");
    }

    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.usmap"),
                tr("US map"),
                tr("One dot per row at its latitude/longitude (lower 48 states)"),
                QUrl(QStringLiteral("qrc:/plugins/usmap_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    // Needs two number columns; the card explains when they aren't found.
    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() >= 2;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/usmap_plugin/UsMap.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        return new MapBackend(model, parent);
    }

    QVariantMap defaultConfig() const override { return {}; }
};

#include "usmapplugin.moc"
