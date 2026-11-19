#include "histogrambackend.h"
#include "ivisualizationplugin.h"

// The histogram as a runtime plugin. View and icon are embedded under
// qrc:/plugins/histogram_plugin/ by datascope_add_plugin().
class HistogramPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.histogram"),
                tr("Histogram"),
                tr("How many rows fall in each value range of the Y column"),
                QUrl(QStringLiteral("qrc:/plugins/histogram_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() > 0;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/histogram_plugin/Histogram.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        return new HistogramBackend(model, parent);
    }

    // bins 0 = automatic
    QVariantMap defaultConfig() const override { return {{QStringLiteral("bins"), 0}}; }
};

#include "histogramplugin.moc"
