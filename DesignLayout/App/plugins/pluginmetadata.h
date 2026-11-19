#ifndef PLUGINMETADATA_H
#define PLUGINMETADATA_H

#include <QString>
#include <QUrl>

// Plain value type describing a plugin (docs/class-spec.md).
// icon is a QUrl (e.g. "qrc:/plugins/bar/icon.svg") instead of the spec's
// QIcon, so QML can use it directly in an Image.
struct PluginMetadata
{
    QString id;          // stable and unique, e.g. "core.viz.bar"
    QString displayName; // shown in the sidebar
    QString description; // tooltip text
    QUrl icon;
    QString version;
};

#endif // PLUGINMETADATA_H
