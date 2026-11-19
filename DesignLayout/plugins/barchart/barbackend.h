#ifndef BARBACKEND_H
#define BARBACKEND_H

#include "xybackend.h"

#include <QStringList>
#include <QVariantList>

// Data behind BarChart.qml: at most `maxBars` bars from the X/Y points
// (see XYBackend). More points than maxBars are split into equal groups;
// each bar is its group's average, labelled with the group's first X.
// Same results as GraphController's old bar chart.
class BarBackend : public XYBackend
{
    Q_OBJECT
    Q_PROPERTY(int maxBars READ maxBars WRITE setMaxBars NOTIFY maxBarsChanged)
    Q_PROPERTY(QStringList labels READ labels NOTIFY changed)
    Q_PROPERTY(QVariantList values READ values NOTIFY changed)
    Q_PROPERTY(bool grouped READ grouped NOTIFY changed)

public:
    explicit BarBackend(QAbstractItemModel *model, QObject *parent = nullptr);

    int maxBars() const { return m_maxBars; }
    void setMaxBars(int bars);

    QStringList labels() const { return m_labels; }
    QVariantList values() const { return m_values; }
    bool grouped() const { return m_grouped; }

signals:
    void maxBarsChanged();

protected:
    void rebuild() override;

private:
    int m_maxBars = 50;
    QStringList m_labels;
    QVariantList m_values;
    bool m_grouped = false;
};

#endif // BARBACKEND_H
