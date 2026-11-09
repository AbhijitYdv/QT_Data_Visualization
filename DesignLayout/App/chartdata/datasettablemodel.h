#ifndef DATASETTABLEMODEL_H
#define DATASETTABLEMODEL_H

#include "graphtable.h"

#include <QAbstractTableModel>
#include <QVector>

// The rows of the current dataset that pass the filter, as a table model for
// a QML TableView. Sorting never touches the data: it reorders a list of row
// numbers, so it is cheap and the GraphTable stays shared with the controller.
class DatasetTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int sortColumn READ sortColumn NOTIFY sortChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending NOTIFY sortChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged) // rows shown

public:
    enum Roles {
        NumericRole = Qt::UserRole + 1, // true for number and date columns (right-aligned)
    };

    explicit DatasetTableModel(QObject *parent = nullptr);

    // Shows `rows` (row numbers into `table`) and keeps the current sort.
    void setSource(const GraphTable &table, const QVector<int> &rows);
    void clear();

    // Click once: ascending; click the same column again: descending.
    Q_INVOKABLE void sortByColumn(int column);

    // Numbers and dates are right-aligned, in the header too.
    Q_INVOKABLE bool isNumericColumn(int column) const;

    int count() const { return static_cast<int>(m_order.size()); }
    int sortColumn() const { return m_sortColumn; }
    bool sortAscending() const { return m_ascending; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void sortChanged();
    void countChanged();

private:
    void applySort();

    GraphTable m_table;
    QVector<int> m_rows;   // filtered rows in file order
    QVector<int> m_order;  // m_rows after sorting
    int m_sortColumn = -1; // -1 = file order
    bool m_ascending = true;
};

#endif // DATASETTABLEMODEL_H
