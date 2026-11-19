#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include "datasettablemodel.h"
#include "graphtable.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>
#include <QVector>

class DatasetHistory;

// Loads a data file (CSV, TSV, TXT, JSON, JSON Lines, GeoJSON, SQLite or XML;
// see parsers/dataparser.h) in the background on a worker thread
// (QtConcurrent), and holds what every card shares:
//  - the X / Y / filter choices from the sidebar;
//  - tableModel: the rows that pass the filter (the data table, and the
//    model every chart plugin draws from - see PluginView.qml);
//  - allRowsModel: every row (the pie uses it when grouping by the filter);
//  - summary: the numbers on the summary cards.
// The charts themselves are plugins (plugins/*); none of their maths is here.
//
// Every successful load is also added to `history` (last 10 datasets this
// session). openFromHistory() brings a dataset back without re-reading the file.
class GraphController : public QObject
{
    Q_OBJECT

    // File info
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY tableChanged)
    Q_PROPERTY(int rowCount READ rowCount NOTIFY tableChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)
    Q_PROPERTY(QStringList fileFilters READ fileFilters CONSTANT) // for the FileDialog

    // Choices for the column pickers
    Q_PROPERTY(QStringList xOptions READ xOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList yOptions READ yOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList filterColumnOptions READ filterColumnOptions NOTIFY tableChanged)
    Q_PROPERTY(QStringList filterValues READ filterValues NOTIFY selectionChanged)

    // Current selection
    Q_PROPERTY(QString xColumnName READ xColumnName WRITE setXColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString yColumnName READ yColumnName WRITE setYColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString filterColumnName READ filterColumnName WRITE setFilterColumnName NOTIFY selectionChanged)
    Q_PROPERTY(QString filterValue READ filterValue WRITE setFilterValue NOTIFY selectionChanged)

    // Summary cards: {count, min, max, mean, median, column} for the Y column
    Q_PROPERTY(QVariantMap summary READ summary NOTIFY dataLoaded)

    // Data table: the filtered rows, sortable by column
    Q_PROPERTY(DatasetTableModel *tableModel READ tableModel CONSTANT)
    // Every row, ignoring the filter (the pie uses it to show all groups
    // when it groups by the filter column)
    Q_PROPERTY(DatasetTableModel *allRowsModel READ allRowsModel CONSTANT)

    // Recent datasets (a list model for the sidebar)
    Q_PROPERTY(QAbstractItemModel *history READ history CONSTANT)
    Q_PROPERTY(int historyCount READ historyCount NOTIFY historyCountChanged)

public:
    enum ColumnType {
        Text = static_cast<int>(CellType::Text),
        Number = static_cast<int>(CellType::Number),
        Date = static_cast<int>(CellType::Date),
    };

    explicit GraphController(QObject *parent = nullptr);

    // Starts loading in the background; returns false if a load is already running.
    Q_INVOKABLE bool loadFile(const QUrl &fileUrl);

    // Re-opens a dataset from the Recent list (row 0 = newest). Replaces the
    // current chart. Returns false while a file is still loading.
    Q_INVOKABLE bool openFromHistory(int row);
    Q_INVOKABLE void removeFromHistory(int row);
    Q_INVOKABLE void clearHistory();

    bool loading() const { return m_loading; }
    QString fileName() const { return m_table.fileName; }
    int rowCount() const { return static_cast<int>(m_table.rows.size()); }
    QString statusMessage() const { return m_statusMessage; }
    QStringList fileFilters() const;

    QStringList xOptions() const;
    QStringList yOptions() const;
    QStringList filterColumnOptions() const;
    QStringList filterValues() const { return m_filterValues; }

    QString xColumnName() const;
    QString yColumnName() const;
    QString filterColumnName() const;
    QString filterValue() const { return m_filterValue; }

    void setXColumnName(const QString &name);
    void setYColumnName(const QString &name);
    void setFilterColumnName(const QString &name);
    void setFilterValue(const QString &value);

    QVariantMap summary() const;
    DatasetTableModel *tableModel() const { return m_tableModel; }
    DatasetTableModel *allRowsModel() const { return m_allRowsModel; }

    QAbstractItemModel *history() const;
    int historyCount() const;

signals:
    void dataLoaded();
    void statusChanged();
    void tableChanged();
    void selectionChanged();
    void loadingChanged();
    void historyCountChanged();

private:
    void applyTable(const GraphTable &table, const DatasetSelection *restore = nullptr);
    void pickDefaultColumns();
    void restoreSelection(const DatasetSelection &selection);
    void rememberSelection();
    void refreshFilterValues();
    void applyFilter();
    void setStatus(const QString &message);
    void setLoading(bool loading);

    GraphTable m_table;

    int m_xCol = -1;      // -1 = use row number
    int m_yCol = -1;      // -1 = nothing to plot
    int m_filterCol = -1; // -1 = no filter
    QString m_filterValue;
    QStringList m_filterValues;

    QVector<int> m_filteredRows;  // row numbers that pass the filter
    QVector<double> m_yValues;    // Y numbers of those rows (summary cards)
    DatasetTableModel *m_tableModel = nullptr; // owned via QObject parent
    DatasetTableModel *m_allRowsModel = nullptr; // owned via QObject parent
    bool m_loading = false;
    QString m_statusMessage;

    DatasetHistory *m_history = nullptr; // owned via QObject parent
    int m_currentEntryId = -1;           // history entry shown right now
};

#endif // GRAPHCONTROLLER_H
