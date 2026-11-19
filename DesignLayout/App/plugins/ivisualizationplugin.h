#ifndef IVISUALIZATIONPLUGIN_H
#define IVISUALIZATIONPLUGIN_H

#include "pluginmetadata.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QtPlugin>

// One implementation per chart type, built as its own shared library and
// loaded at runtime with QPluginLoader. The core app never includes a
// concrete plugin header - only this one.
class IVisualizationPlugin
{
public:
    virtual ~IVisualizationPlugin() = default;

    virtual PluginMetadata metadata() const = 0;

    // Cheap check: can this chart show the model? (e.g. map needs lat/lon)
    virtual bool canVisualize(QAbstractItemModel *model) const = 0;

    // qrc:/ path inside the plugin; the dashboard card's Loader shows it.
    virtual QUrl qmlSource() const = 0;

    // QObject whose properties the QML view binds to. Must follow the
    // model's dataChanged / rowsInserted / rowsRemoved / modelReset.
    virtual QObject *createBackend(QAbstractItemModel *model, QObject *parent) = 0;

    // Axes, bins, thresholds... flat so a layout can be saved as JSON.
    virtual QVariantMap defaultConfig() const = 0;
};

#define IVisualizationPlugin_iid "com.datascope.IVisualizationPlugin/1.0"
Q_DECLARE_INTERFACE(IVisualizationPlugin, IVisualizationPlugin_iid)

#endif // IVISUALIZATIONPLUGIN_H
