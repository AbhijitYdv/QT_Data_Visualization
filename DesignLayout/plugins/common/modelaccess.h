#ifndef MODELACCESS_H
#define MODELACCESS_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QVector>

// Small helpers for chart plugins that read the dataset model.
// Roles are looked up by name, so plugins never include DatasetTableModel:
//   "value"      cell as a double (dates: ms since epoch), invalid if not a number
//   "sourceRow"  row number in the file
//   "columnType" headerData: CellType as int (0 text, 1 number, 2 date)
namespace ModelAccess {

enum ColumnType { Text = 0, Number = 1, Date = 2 };

inline int role(const QAbstractItemModel *model, const char *name)
{
    if (!model)
        return -1;
    const QHash<int, QByteArray> names = model->roleNames();
    for (auto it = names.begin(); it != names.end(); ++it) {
        if (it.value() == name)
            return it.key();
    }
    return -1;
}

inline QStringList headers(const QAbstractItemModel *model)
{
    QStringList out;
    if (!model)
        return out;
    for (int c = 0; c < model->columnCount(); ++c)
        out << model->headerData(c, Qt::Horizontal).toString();
    return out;
}

inline int column(const QAbstractItemModel *model, const QString &name)
{
    return name.isEmpty() ? -1 : static_cast<int>(headers(model).indexOf(name));
}

// -1 if the model doesn't publish column types.
inline QVector<int> types(const QAbstractItemModel *model)
{
    QVector<int> out;
    const int typeRole = role(model, "columnType");
    if (!model || typeRole < 0)
        return out;
    for (int c = 0; c < model->columnCount(); ++c)
        out << model->headerData(c, Qt::Horizontal, typeRole).toInt();
    return out;
}

// Headers of the columns of one type, in order.
inline QStringList columnsOfType(const QAbstractItemModel *model, int type)
{
    const QStringList names = headers(model);
    const QVector<int> t = types(model);
    QStringList out;
    for (int c = 0; c < t.size(); ++c)
        if (t.at(c) == type)
            out << names.value(c);
    return out;
}

// Connects every "model changed" signal to `slot` (a lambda or functor).
template <typename Receiver, typename Slot>
void onAnyChange(QAbstractItemModel *model, Receiver *receiver, Slot slot)
{
    if (!model)
        return;
    QObject::connect(model, &QAbstractItemModel::modelReset, receiver, slot);
    QObject::connect(model, &QAbstractItemModel::dataChanged, receiver, slot);
    QObject::connect(model, &QAbstractItemModel::rowsInserted, receiver, slot);
    QObject::connect(model, &QAbstractItemModel::rowsRemoved, receiver, slot);
    QObject::connect(model, &QAbstractItemModel::layoutChanged, receiver, slot);
    QObject::connect(model, &QAbstractItemModel::headerDataChanged, receiver, slot);
}

} // namespace ModelAccess

#endif // MODELACCESS_H
