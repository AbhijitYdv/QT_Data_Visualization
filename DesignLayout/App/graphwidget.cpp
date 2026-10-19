#include "GraphWidget.h"

#include <QLineSeries>
#include <QChart>
#include <QPainter>
#include <QPointF>
GraphWidget::GraphWidget(QWidget *parent)
    : QChartView(parent)

{

    QLineSeries *series = new QLineSeries;

    series -> append(0,0);
    *series << QPointF(0,1)<< QPointF(0,11)<< QPointF(10,1)<< QPointF(13,13);

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Line Chart");

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
}



