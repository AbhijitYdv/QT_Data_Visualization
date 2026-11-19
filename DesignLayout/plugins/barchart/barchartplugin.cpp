#include "barbackend.h"
#include "ivisualizationplugin.h"

// The bar chart as a runtime plugin. The view (BarChart.qml) and icon are
// embedded under qrc:/plugins/barchart_plugin/ by datascope_add_plugin().
class BarChartPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.bar"),
                tr("Bar chart"),
                tr("Y values as bars; large data is grouped into averaged bars"),
                QUrl(QStringLiteral("qrc:/plugins/barchart_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    // Needs at least one column; whether Y holds numbers is checked when drawing.
    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() > 0;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/barchart_plugin/BarChart.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        auto *backend = new BarBackend(model, parent);
        backend->setMaxBars(defaultConfig().value(QStringLiteral("maxBars")).toInt());
        return backend;
    }

    QVariantMap defaultConfig() const override
    {
        return {{QStringLiteral("maxBars"), 50}};
    }
};

#include "barchartplugin.moc"
