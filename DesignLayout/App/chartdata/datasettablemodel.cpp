#include "datasettablemodel.h"
#include "parserutils.h"

#include <algorithm>
#include <cmath>

DatasetTableModel::DatasetTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void DatasetTableModel::setSource(const GraphTable &table, const QVector<int> &rows)
{
    beginResetModel();
    const bool sameColumns = table.headers == m_table.headers;
    m_table = table;
    m_rows = rows;
    if (!sameColumns) { // a different dataset: back to file order
        m_sortColumn = -1;
        m_ascending = true;
    }
    applySort();
    endResetModel();
    emit countChanged();
    if (!sameColumns)
        emit sortChanged();
}

void DatasetTableModel::clear()
{
    setSource(GraphTable(), {});
}

void DatasetTableModel::sortByColumn(int column)
{
    if (column < 0 || column >= m_table.headers.size())
        return;
    if (column == m_sortColumn)
        m_ascending = !m_ascending;
    else {
        m_sortColumn = column;
        m_ascending = true;
    }
    beginResetModel();
    applySort();
    endResetModel();
    emit sortChanged();
}

// Numbers and dates sort by value, text alphabetically (ignoring case).
// Cells that are empty or don't parse always go last, in either direction.
void DatasetTableModel::applySort()
{
    m_order = m_rows;
    if (m_sortColumn < 0)
        return;

    const int col = m_sortColumn;
    const int type = m_table.types.value(col);
    const bool asc = m_ascending;

    if (type == static_cast<int>(CellType::Text)) {
        std::stable_sort(m_order.begin(), m_order.end(), [&](int a, int b) {
            const QString &x = m_table.rows.at(a).at(col);
            const QString &y = m_table.rows.at(b).at(col);
            if (x.isEmpty() != y.isEmpty())
                return y.isEmpty();
            const int c = x.compare(y, Qt::CaseInsensitive);
            return asc ? c < 0 : c > 0;
        });
        return;
    }

    // Parse each cell once, not once per comparison.
    const bool isDate = type == static_cast<int>(CellType::Date);
    QVector<double> key(m_table.rows.size(), NAN); // indexed by row number
    for (int r : std::as_const(m_order)) {
        double v = NAN;
        const QString &cell = m_table.rows.at(r).at(col);
        if (isDate ? !Parsers::parseDate(cell, &v) : !Parsers::parseNumber(cell, &v))
            v = NAN;
        key[r] = v;
    }
    std::stable_sort(m_order.begin(), m_order.end(), [&](int a, int b) {
        const double x = key.at(a), y = key.at(b);
        if (std::isnan(x) != std::isnan(y))
            return std::isnan(y);
        return asc ? x < y : x > y;
    });
}

bool DatasetTableModel::isNumericColumn(int column) const
{
    return column >= 0 && column < m_table.types.size()
           && m_table.types.at(column) != static_cast<int>(CellType::Text);
}

int DatasetTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_order.size());
}

int DatasetTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_table.headers.size());
}

QVariant DatasetTableModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return {};
    switch (role) {
    case Qt::DisplayRole:
        return m_table.rows.at(m_order.at(index.row())).at(index.column());
    case NumericRole:
        return isNumericColumn(index.column());
    default:
        return {};
    }
}

QVariant DatasetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    if (orientation == Qt::Horizontal)
        return m_table.headers.value(section);
    return section + 1;
}

QHash<int, QByteArray> DatasetTableModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractTableModel::roleNames();
    names.insert(NumericRole, "numeric");
    return names;
}
