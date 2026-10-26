#include "datasethistory.h"


DatasetHistory::DatasetHistory(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DatasetHistory::add(const GraphTable &table)
{
    Entry entry;
    entry.id = m_nextId++;
    entry.table = table;
    entry.loadedAt = QDateTime::currentDateTime();

    // Same file imported again: keep its saved column choices, drop the old entry.
    const int existing = rowOfPath(table.filePath);
    if (existing >= 0) {
        entry.selection = m_entries.at(existing).selection;
        beginRemoveRows(QModelIndex(), existing, existing);
        m_entries.removeAt(existing);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(entry);
    endInsertRows();

    // Enforce the cap by dropping the oldest entries.
    if (m_entries.size() > kMaxEntries) {
        const int first = kMaxEntries;
        const int last = static_cast<int>(m_entries.size()) - 1;
        beginRemoveRows(QModelIndex(), first, last);
        while (m_entries.size() > kMaxEntries)
            m_entries.removeLast();
        endRemoveRows();
    }

    emit countChanged();
    return entry.id;
}

const DatasetHistory::Entry *DatasetHistory::entryAt(int row) const
{
    if (row < 0 || row >= m_entries.size())
        return nullptr;
    return &m_entries.at(row);
}

const DatasetHistory::Entry *DatasetHistory::entryById(int id) const
{
    return entryAt(rowOfId(id));
}

void DatasetHistory::setSelection(int id, const DatasetSelection &selection)
{
    const int row = rowOfId(id);
    if (row >= 0)
        m_entries[row].selection = selection;
}

void DatasetHistory::setCurrentId(int id)
{
    if (id == m_currentId)
        return;
    const int oldRow = rowOfId(m_currentId);
    m_currentId = id;
    notifyCurrent(oldRow);
    notifyCurrent(rowOfId(m_currentId));
}

void DatasetHistory::removeAt(int row)
{
    if (row < 0 || row >= m_entries.size())
        return;
    if (m_entries.at(row).id == m_currentId)
        m_currentId = -1;
    beginRemoveRows(QModelIndex(), row, row);
    m_entries.removeAt(row);
    endRemoveRows();
    emit countChanged();
}

void DatasetHistory::clear()
{
    if (m_entries.isEmpty())
        return;
    beginResetModel();
    m_entries.clear();
    m_currentId = -1;
    endResetModel();
    emit countChanged();
}

int DatasetHistory::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QVariant DatasetHistory::data(const QModelIndex &index, int role) const
{
    const Entry *entry = entryAt(index.row());
    if (!entry || index.column() != 0)
        return {};

    switch (role) {
    case Qt::DisplayRole:
    case FileNameRole:
        return entry->table.fileName;
    case FilePathRole:
        return entry->table.filePath;
    case RowCountRole:
        return static_cast<int>(entry->table.rows.size());
    case ColumnCountRole:
        return static_cast<int>(entry->table.headers.size());
    case LoadedAtRole:
        return entry->loadedAt.time().toString(QStringLiteral("h:mm AP"));
    case SummaryRole:
        return tr("%1 rows · %2 cols · %3")
            .arg(entry->table.rows.size())
            .arg(entry->table.headers.size())
            .arg(entry->loadedAt.time().toString(QStringLiteral("h:mm AP")));
    case IsCurrentRole:
        return entry->id == m_currentId;
    default:
        return {};
    }
}

QHash<int, QByteArray> DatasetHistory::roleNames() const
{
    return {
        {FileNameRole, "fileName"},
        {FilePathRole, "filePath"},
        {RowCountRole, "rows"},
        {ColumnCountRole, "columns"},
        {LoadedAtRole, "loadedAt"},
        {SummaryRole, "summary"},
        {IsCurrentRole, "isCurrent"},
    };
}

int DatasetHistory::rowOfId(int id) const
{
    if (id < 0)
        return -1;
    for (int i = 0; i < m_entries.size(); ++i)
        if (m_entries.at(i).id == id)
            return i;
    return -1;
}

int DatasetHistory::rowOfPath(const QString &path) const
{
    if (path.isEmpty())
        return -1;
    for (int i = 0; i < m_entries.size(); ++i)
        if (m_entries.at(i).table.filePath == path)
            return i;
    return -1;
}

void DatasetHistory::notifyCurrent(int row)
{
    if (row < 0)
        return;
    const QModelIndex idx = index(row, 0);
    emit dataChanged(idx, idx, {IsCurrentRole});
}
