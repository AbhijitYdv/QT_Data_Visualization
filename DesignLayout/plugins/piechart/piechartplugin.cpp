#include "ivisualizationplugin.h"
#include "piebackend.h"

// The pie (donut) chart as a runtime plugin. View and icon are embedded
// under qrc:/plugins/piechart_plugin/ by datascope_add_plugin().
class PieChartPlugin : public QObject, public IVisualizationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IVisualizationPlugin_iid)
    Q_INTERFACES(IVisualizationPlugin)

public:
    PluginMetadata metadata() const override
    {
        return {QStringLiteral("core.viz.pie"),
                tr("Pie chart"),
                tr("Share of rows (or of the Y total) per category of a text column"),
                QUrl(QStringLiteral("qrc:/plugins/piechart_plugin/icon.svg")),
                QStringLiteral("1.0")};
    }

    // Needs a text column to group by; the card explains when there isn't one.
    bool canVisualize(QAbstractItemModel *model) const override
    {
        return model && model->columnCount() > 0;
    }

    QUrl qmlSource() const override
    {
        return QUrl(QStringLiteral("qrc:/plugins/piechart_plugin/PieChart.qml"));
    }

    QObject *createBackend(QAbstractItemModel *model, QObject *parent) override
    {
        return new PieBackend(model, parent);
    }

    QVariantMap defaultConfig() const override
    {
        return {{QStringLiteral("maxSlices"), 7}};
    }
};

#include "piechartplugin.moc"
