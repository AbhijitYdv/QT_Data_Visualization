#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "pluginmetadata.h"

#include <QHash>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVector>

class IVisualizationPlugin;
class QPluginLoader;

// Finds chart plugins at runtime (docs/class-spec.md, "PluginManager").
//
//  - loadFrom(dir) tries every shared library in `dir`. A file that is not
//    a library, not a Qt plugin, the wrong interface, or a duplicate id
//    emits pluginFailed and is skipped - it never stops startup.
//  - Calling loadFrom twice on the same directory is safe (already loaded
//    paths are skipped).
//  - loadStatic() picks up plugins linked into the app with Q_IMPORT_PLUGIN.
//  - `plugins` is a list of {id, name, description, icon, version} maps
//    for QML (sidebar / "add chart" menu), in load order.
class PluginManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList plugins READ pluginList NOTIFY pluginsChanged)
    Q_PROPERTY(int count READ count NOTIFY pluginsChanged)

public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager() override;

    // Where the app looks by default:
    //   macOS bundle: <App>.app/Contents/PlugIns/datascope
    //   otherwise:    <folder of the executable>/plugins
    static QString defaultPluginDir();

    void loadFrom(const QString &directory);
    void loadStatic();

    QVector<IVisualizationPlugin *> visualizations() const { return m_visualizations; }
    IVisualizationPlugin *visualization(const QString &id) const;

    // For QML
    Q_INVOKABLE QUrl qmlSource(const QString &id) const;
    Q_INVOKABLE QVariantMap defaultConfig(const QString &id) const;
    Q_INVOKABLE bool has(const QString &id) const { return m_byId.contains(id); }

    // Asks plugin `id` for its backend. `model` must be a QAbstractItemModel
    // (e.g. graphController.tableModel). The backend is owned by `parent`
    // (pass the view item from QML so it is deleted with the view).
    // Returns null if the plugin is missing or the model is not a model.
    Q_INVOKABLE QObject *createBackend(const QString &id, QObject *model, QObject *parent);

    QVariantList pluginList() const;
    int count() const { return static_cast<int>(m_visualizations.size()); }
    QStringList errors() const { return m_errors; } // "path: reason", for logs/tests

signals:
    void pluginLoaded(const PluginMetadata &metadata);
    void pluginFailed(const QString &path, const QString &reason);
    void pluginsChanged();

private:
    bool add(QObject *instance, const QString &source);
    void fail(const QString &path, const QString &reason);

    QVector<IVisualizationPlugin *> m_visualizations;
    QHash<QString, IVisualizationPlugin *> m_byId;
    QSet<QString> m_triedPaths;
    QVector<QPluginLoader *> m_loaders; // keep libraries loaded
    QStringList m_errors;
};

#endif // PLUGINMANAGER_H
