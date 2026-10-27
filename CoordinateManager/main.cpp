#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QScatterSeries>
#include <QChart>
#include <QChartView>
#include <QApplication>
//using namespace QtCharts;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();

}
