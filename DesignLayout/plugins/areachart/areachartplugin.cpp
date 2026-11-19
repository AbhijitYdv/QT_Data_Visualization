#include "ivisualizationplugin.h"
#include "seriesbackend.h"

// Area chart as a runtime plugin. View and icon are embedded under
// qrc:/plugins/areachart_plugin/ by datascope_add_plugin().
class AreaChartPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.area"),
                tr("Area chart"),
                tr("Y against X, filled down to zero"),
                QUrl(QStringLiteral("qrc:/plugins/areachart_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() > 0;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/areachart_plugin/AreaChart.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        auto *backend = new SeriesBackend(model, parent);
        backend->setMode(SeriesBackend::Thin);
        backend->setMaxPoints(defaultConfig().value(QStringLiteral("maxPoints")).toInt());
        return backend;
    }

    QVariantMap defaultConfig() const override
    {
        return {{QStringLiteral("maxPoints"), 2000}};
    }
};

#include "areachartplugin.moc"
