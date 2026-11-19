#include "ivisualizationplugin.h"
#include "seriesbackend.h"

// Scatter plot as a runtime plugin. View and icon are embedded under
// qrc:/plugins/scatterplot_plugin/ by datascope_add_plugin().
class ScatterPlotPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.scatter"),
                tr("Scatter plot"),
                tr("One dot per row; large data is an even sample"),
                QUrl(QStringLiteral("qrc:/plugins/scatterplot_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() > 0;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/scatterplot_plugin/ScatterPlot.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        auto *backend = new SeriesBackend(model, parent);
        backend->setMode(SeriesBackend::Sample);
        backend->setMaxPoints(defaultConfig().value(QStringLiteral("maxPoints")).toInt());
        return backend;
    }

    QVariantMap defaultConfig() const override
    {
        return {{QStringLiteral("maxPoints"), 5000}};
    }
};

#include "scatterplotplugin.moc"
