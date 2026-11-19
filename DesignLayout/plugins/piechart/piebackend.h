#ifndef PIEBACKEND_H
#define PIEBACKEND_H

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QVariantList>

// Data behind PieChart.qml: share of rows (or of the Y total) per category
// of a text column (ChartData::groups: 7 largest + "Other"). Same as the old
// card:
//  - starts on the filter column, or else the first text column; a new
//    dataset goes back to that;
//  - grouping by the filter column shows every group (from allRowsModel)
//    and `highlight` is the filtered one.
class PieBackend : public QObject
{
    Q_OBJECT
    // Set by the user (pickers in the card)
    Q_PROPERTY(QString groupColumn READ groupColumn WRITE setGroupColumn NOTIFY changed)
    Q_PROPERTY(bool sumsY READ sumsY WRITE setSumsY NOTIFY changed)
    // Set by the dashboard (PluginView)
    Q_PROPERTY(QString yColumn READ yColumn WRITE setYColumn NOTIFY inputsChanged)
    Q_PROPERTY(QString filterColumn READ filterColumn WRITE setFilterColumn NOTIFY inputsChanged)
    Q_PROPERTY(QString filterValue READ filterValue WRITE setFilterValue NOTIFY inputsChanged)
    Q_PROPERTY(QAbstractItemModel *allRowsModel READ allRowsModel WRITE setAllRowsModel NOTIFY inputsChanged)

    Q_PROPERTY(QStringList groupOptions READ groupOptions NOTIFY changed) // text columns
    Q_PROPERTY(QStringList labels READ labels NOTIFY changed)
    Q_PROPERTY(QVariantList values READ values NOTIFY changed)
    Q_PROPERTY(QVariantList rows READ rows NOTIFY changed)
    Q_PROPERTY(int highlight READ highlight NOTIFY changed)          // slice index or -1
    Q_PROPERTY(bool hasOther READ hasOther NOTIFY changed)           // last slice is "Other"
    Q_PROPERTY(double total READ total NOTIFY changed)
    Q_PROPERTY(QString valueName READ valueName NOTIFY changed)      // "Rows" / "Sum of y"
    Q_PROPERTY(int rowCount READ rowCount NOTIFY changed)            // filtered rows

public:
    explicit PieBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    QString groupColumn() const { return m_effectiveGroup; }
    void setGroupColumn(const QString &name);
    bool sumsY() const { return m_sumsY; }
    void setSumsY(bool sums);
    QString yColumn() const { return m_yColumn; }
    void setYColumn(const QString &name);
    QString filterColumn() const { return m_filterColumn; }
    void setFilterColumn(const QString &name);
    QString filterValue() const { return m_filterValue; }
    void setFilterValue(const QString &value);
    QAbstractItemModel *allRowsModel() const { return m_allRows; }
    void setAllRowsModel(QAbstractItemModel *model);

    QStringList groupOptions() const { return m_groupOptions; }
    QStringList labels() const { return m_labels; }
    QVariantList values() const { return m_values; }
    QVariantList rows() const { return m_rows; }
    int highlight() const { return m_highlight; }
    bool hasOther() const { return m_hasOther; }
    double total() const { return m_total; }
    QString valueName() const { return m_valueName; }
    int rowCount() const { return m_model ? m_model->rowCount() : 0; }

    Q_INVOKABLE void refresh();

signals:
    void inputsChanged();
    void changed();

private:
    void scheduleRefresh();

    QPointer<QAbstractItemModel> m_model;
    QPointer<QAbstractItemModel> m_allRows;
    QString m_chosenGroup; // empty = automatic
    QString m_effectiveGroup;
    bool m_sumsY = false;
    QString m_yColumn;
    QString m_filterColumn;
    QString m_filterValue;
    QStringList m_lastHeaders;
    bool m_queued = false;

    QStringList m_groupOptions;
    QStringList m_labels;
    QVariantList m_values;
    QVariantList m_rows;
    int m_highlight = -1;
    bool m_hasOther = false;
    double m_total = 0.0;
    QString m_valueName;
};

#endif // PIEBACKEND_H
