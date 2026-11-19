#include "ivisualizationplugin.h"

class DemoPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)
public:
    PluginMetadata metadata() const override
    {
        return {"test.viz.demo", "Demo", "Built with datascope_add_plugin",
                QUrl("qrc:/plugins/demo_plugin/icon.svg"), "1.0"};
    }
    bool canVisualize(QAbstractItemModel *m) const override { return m; }
    QUrl qmlSource() const override { return QUrl("qrc:/plugins/demo_plugin/Demo.qml"); }
    QObject *createBackend(QAbstractItemModel *, QObject *parent) override
    {
        return new QObject(parent);
    }
    QVariantMap defaultConfig() const override { return {}; }
};

#include "demoplugin.moc"
