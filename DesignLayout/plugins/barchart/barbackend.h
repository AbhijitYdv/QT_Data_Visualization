#ifndef BARBACKEND_H
#define BARBACKEND_H

#include <QAbstractItemModel>
#include <QPointer>
#include <QObject>
#include <QStringList>
#include <QVariantList>

// Data behind BarChart.qml. Reads the filtered dataset model and builds
// at most `maxBars` bars from two columns:
//  - X sorted ascending (or file order when xColumn is not a column,
//    e.g. "Row #"); rows whose Y (or X) doesn't parse are skipped;
//  - more points than maxBars: split into equal groups, each bar is the
//    group's average and is labelled with the group's first X.
// Same results as GraphController's old bar chart.
//
// Only uses the model through roles looked up by name ("value",
// "sourceRow", "columnType"), so it does not depend on DatasetTableModel.
class BarBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString xColumn READ xColumn WRITE setXColumn NOTIFY xColumnChanged)
    Q_PROPERTY(QString yColumn READ yColumn WRITE setYColumn NOTIFY yColumnChanged)
    Q_PROPERTY(int maxBars READ maxBars WRITE setMaxBars NOTIFY maxBarsChanged)

    Q_PROPERTY(QStringList labels READ labels NOTIFY changed)
    Q_PROPERTY(QVariantList values READ values NOTIFY changed)
    Q_PROPERTY(double minY READ minY NOTIFY changed) // over all points, not bars
    Q_PROPERTY(double maxY READ maxY NOTIFY changed)
    Q_PROPERTY(bool grouped READ grouped NOTIFY changed)
    Q_PROPERTY(bool xIsDate READ xIsDate NOTIFY changed)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY changed)

public:
    explicit BarBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    QString xColumn() const { return m_xColumn; }
    QString yColumn() const { return m_yColumn; }
    int maxBars() const { return m_maxBars; }
    void setXColumn(const QString &name);
    void setYColumn(const QString &name);
    void setMaxBars(int bars);

    QStringList labels() const { return m_labels; }
    QVariantList values() const { return m_values; }
    double minY() const { return m_minY; }
    double maxY() const { return m_maxY; }
    bool grouped() const { return m_grouped; }
    bool xIsDate() const { return m_xIsDate; }
    int pointCount() const { return m_pointCount; }

    // Rebuilds the bars now (normally done automatically).
    Q_INVOKABLE void refresh();

signals:
    void xColumnChanged();
    void yColumnChanged();
    void maxBarsChanged();
    void changed();

private:
    void scheduleRefresh();
    int columnIndex(const QString &name) const;
    int role(const char *name) const;

    QPointer<QAbstractItemModel> m_model;
    QString m_xColumn;
    QString m_yColumn;
    int m_maxBars = 50;
    bool m_refreshQueued = false;

    QStringList m_labels;
    QVariantList m_values;
    double m_minY = 0.0;
    double m_maxY = 1.0;
    bool m_grouped = false;
    bool m_xIsDate = false;
    int m_pointCount = 0;
};

#endif // BARBACKEND_H
