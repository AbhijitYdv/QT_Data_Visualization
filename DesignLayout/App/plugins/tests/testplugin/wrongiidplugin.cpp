#include <QObject>
#include <QtPlugin>

// A valid Qt plugin, but for some other interface.
class WrongIidPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.SomethingElse/1.0")
};

#include "wrongiidplugin.moc"
