#ifndef DATASETHISTORY_H
#define DATASETHISTORY_H

#include "graphtable.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>

// In-session history of loaded datasets (cleared when the app closes).
//
// - One entry per dataset (per file). Re-importing the same file replaces
//   its entry and moves it to the top instead of adding a duplicate.
// - Newest first, capped at kMaxEntries; the oldest entry is dropped.
// - Each entry keeps the parsed data plus the last X / Y / filter choices,
//   so reopening it brings the charts back exactly as they were.
class DatasetHistory : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    static constexpr int kMaxEntries = 10;

    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        RowCountRole,
        ColumnCountRole,
        LoadedAtRole,
        SummaryRole,
        IsCurrentRole,
    };

    struct Entry
    {
        int id = -1;
        GraphTable table;
        DatasetSelection selection;
        QDateTime loadedAt;
    };

    explicit DatasetHistory(QObject *parent = nullptr);

    // Adds a dataset at the top and returns its id.
    int add(const GraphTable &table);

    // nullptr if the row or id doesn't exist.
    const Entry *entryAt(int row) const;
    const Entry *entryById(int id) const;

    void setSelection(int id, const DatasetSelection &selection);
    void setCurrentId(int id);
    int currentId() const { return m_currentId; }

    void removeAt(int row);
    void clear();
    int count() const { return static_cast<int>(m_entries.size()); }

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void countChanged();

private:
    int rowOfId(int id) const;
    int rowOfPath(const QString &path) const;
    void notifyCurrent(int row);

    QList<Entry> m_entries; // index 0 = newest
    int m_nextId = 1;
    int m_currentId = -1;
};

#endif // DATASETHISTORY_H
