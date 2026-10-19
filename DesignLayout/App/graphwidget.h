
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QChartView>

class GraphWidget : public QChartView
{
public:
    explicit GraphWidget(QWidget *parent = nullptr);
};

#endif
