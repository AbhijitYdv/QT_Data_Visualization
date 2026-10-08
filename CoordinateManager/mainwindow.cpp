#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLineSeries *series = new QLineSeries;

    series -> append(0,0);
    *series << QPointF(0,1)<< QPointF(0,11)<< QPointF(10,1)<< QPointF(13,13);

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Line Chart");

    QChartView *chartVeiw = new QChartView(chart);
    chartVeiw->setRenderHint(QPainter::Antialiasing);
    chartVeiw->setParent(ui->horizontalFrame);
}

MainWindow::~MainWindow()
{

    delete ui;
}
