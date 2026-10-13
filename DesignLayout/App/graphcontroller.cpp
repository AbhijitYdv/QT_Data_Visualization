
#include "graphcontroller.h"
#include <QDebug>
#include <fstream>
#include <string>
GraphController::GraphController(QObject *parent)
    : QObject(parent)
{
}

void GraphController::addPoint(double x, double y)
{
    points.append(QPointF(x, y));
    emit pointAdded(x, y);
}

void GraphController::clearGraph()
{
    points.clear();
    emit graphCleared();
}

void GraphController::loadDefaultPoints()
{
    clearGraph();
    std::ifstream f("C:\\default.txt");
    std::string text;
    double x, y;

    while (f >> x >> y) {
        addPoint(x, y);
    }

    f.close();

}
