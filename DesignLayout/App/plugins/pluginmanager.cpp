#include "pluginmanager.h"
#include "ivisualizationplugin.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QLibrary>
#include <QPluginLoader>
#include <QVariantMap>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<PluginMetadata>("PluginMetadata");
}

// Libraries stay loaded until the app quits: QML views and backends created
// from a plugin may still be alive while this object is destroyed, so
// unloading here could pull code out from under them.
PluginManager::~PluginManager() = default;

QString PluginManager::defaultPluginDir()
{
    QDir dir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACOS
    if (dir.dirName() == QLatin1String("MacOS")) { // inside an .app bundle
        dir.cdUp();
        return dir.absoluteFilePath(QStringLiteral("PlugIns/datascope"));
    }
#endif
    return dir.absoluteFilePath(QStringLiteral("plugins"));
}

void PluginManager::loadFrom(const QString &directory)
{
    const QDir dir(directory);
    if (!dir.exists()) {
        fail(directory, tr("Plugin folder does not exist"));
        return;
    }

    const QFileInfoList files =
        dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    bool changed = false;

    for (const QFileInfo &file : files) {
        const QString path = file.canonicalFilePath();
        if (m_triedPaths.contains(path))
            continue;
        if (!QLibrary::isLibrary(path)) // no hardcoded .so/.dll/.dylib
            continue;
        m_triedPaths.insert(path);

        auto *loader = new QPluginLoader(path, this);

        // Check the IID from the metadata first: this reads the file
        // without running any of its code.
        const QString iid = loader->metaData().value(QStringLiteral("IID")).toString();
        if (iid.isEmpty()) {
            fail(path, tr("Not a Qt plugin"));
            delete loader;
            continue;
        }
        if (iid != QLatin1String(IVisualizationPlugin_iid)) {
            fail(path, tr("Wrong interface %1 (expected %2)")
                           .arg(iid, QLatin1String(IVisualizationPlugin_iid)));
            delete loader;
            continue;
        }

        QObject *instance = loader->instance();
        if (!instance) {
            fail(path, loader->errorString());
            delete loader;
            continue;
        }
        if (add(instance, path)) {
            m_loaders.append(loader);
            changed = true;
        } else {
            loader->unload();
            delete loader;
        }
    }

    if (changed)
        emit pluginsChanged();
}

void PluginManager::loadStatic()
{
    bool changed = false;
    const QObjectList statics = QPluginLoader::staticInstances();
    for (QObject *instance : statics) {
        if (!qobject_cast<IVisualizationPlugin *>(instance))
            continue; // e.g. Qt's own static plugins
        const QString source = QStringLiteral("static:") + instance->metaObject()->className();
        if (m_triedPaths.contains(source))
            continue;
        m_triedPaths.insert(source);
        changed |= add(instance, source);
    }
    if (changed)
        emit pluginsChanged();
}

bool PluginManager::add(QObject *instance, const QString &source)
{
    auto *viz = qobject_cast<IVisualizationPlugin *>(instance);
    if (!viz) {
        fail(source, tr("Does not implement IVisualizationPlugin"));
        return false;
    }
    const PluginMetadata meta = viz->metadata();
    if (meta.id.isEmpty()) {
        fail(source, tr("Plugin has no id"));
        return false;
    }
    if (m_byId.contains(meta.id)) {
        fail(source, tr("Duplicate plugin id \"%1\"").arg(meta.id));
        return false;
    }

    m_visualizations.append(viz);
    m_byId.insert(meta.id, viz);
    emit pluginLoaded(meta);
    return true;
}

void PluginManager::fail(const QString &path, const QString &reason)
{
    m_errors.append(path + QStringLiteral(": ") + reason);
    qWarning().noquote() << "Plugin skipped:" << path << "-" << reason;
    emit pluginFailed(path, reason);
}

IVisualizationPlugin *PluginManager::visualization(const QString &id) const
{
    return m_byId.value(id, nullptr);
}

QUrl PluginManager::qmlSource(const QString &id) const
{
    IVisualizationPlugin *viz = visualization(id);
    return viz ? viz->qmlSource() : QUrl();
}

QVariantMap PluginManager::defaultConfig(const QString &id) const
{
    IVisualizationPlugin *viz = visualization(id);
    return viz ? viz->defaultConfig() : QVariantMap();
}

QVariantList PluginManager::pluginList() const
{
    QVariantList list;
    for (IVisualizationPlugin *viz : m_visualizations) {
        const PluginMetadata m = viz->metadata();
        list.append(QVariantMap{
            {QStringLiteral("id"), m.id},
            {QStringLiteral("name"), m.displayName},
            {QStringLiteral("description"), m.description},
            {QStringLiteral("icon"), m.icon},
            {QStringLiteral("version"), m.version},
        });
    }
    return list;
}
