#include "ivisualizationplugin.h"

class GoodPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)
public:
    PluginMetadata metadata() const override
    {
        return {"test.viz.good", "Good", "Loads fine", QUrl("qrc:/good.svg"), "1.0"};
    }
    bool canVisualize(QAbstractItemModel *m) const override { return m; }
    QUrl qmlSource() const override { return QUrl("qrc:/good/Good.qml"); }
    QObject *createBackend(QAbstractItemModel *, QObject *parent) override
    {
        return new QObject(parent);
    }
    QVariantMap defaultConfig() const override { return {{"bins", 12}}; }
};

#include "goodplugin.moc"
