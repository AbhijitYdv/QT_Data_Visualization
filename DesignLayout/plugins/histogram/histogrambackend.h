#ifndef HISTOGRAMBACKEND_H
#define HISTOGRAMBACKEND_H

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QVariantList>
#include <QVector>

// Data behind Histogram.qml: how many filtered rows fall in each value
// range of the Y column (ChartData::histogram, same as the old card).
// bins = 0 means automatic (Sturges' rule); a new dataset goes back to auto.
class HistogramBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString yColumn READ yColumn WRITE setYColumn NOTIFY yColumnChanged)
    Q_PROPERTY(int bins READ bins WRITE setBins NOTIFY binsChanged)

    Q_PROPERTY(int autoBins READ autoBins NOTIFY changed)
    Q_PROPERTY(int binCount READ binCount NOTIFY changed)       // bins actually drawn
    Q_PROPERTY(QStringList labels READ labels NOTIFY changed)   // "120–135" per bin
    Q_PROPERTY(QVariantList counts READ counts NOTIFY changed)
    Q_PROPERTY(QVariantList edges READ edges NOTIFY changed)    // binCount + 1
    Q_PROPERTY(int maxCount READ maxCount NOTIFY changed)

public:
    explicit HistogramBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    QString yColumn() const { return m_yColumn; }
    void setYColumn(const QString &name);
    int bins() const { return m_bins; }
    void setBins(int bins);

    int autoBins() const { return m_autoBins; }
    int binCount() const { return static_cast<int>(m_counts.size()); }
    QStringList labels() const { return m_labels; }
    QVariantList counts() const;
    QVariantList edges() const;
    int maxCount() const { return m_maxCount; }

    // Bars as one outline for an AreaSeries' upper LineSeries (one repaint).
    Q_INVOKABLE void fillSeries(QObject *series) const;
    Q_INVOKABLE void refresh();

signals:
    void yColumnChanged();
    void binsChanged();
    void changed();

private:
    void scheduleRefresh();

    QPointer<QAbstractItemModel> m_model;
    QString m_yColumn;
    int m_bins = 0;
    bool m_queued = false;
    QStringList m_lastHeaders;

    int m_autoBins = 0;
    QVector<double> m_edges;
    QVector<int> m_counts;
    QStringList m_labels;
    int m_maxCount = 0;
};

#endif // HISTOGRAMBACKEND_H
