
#include "graphcontroller.h"

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

    addPoint(0, 0);
    addPoint(0, 1);
    addPoint(0, 11);
    addPoint(10, 1);
    addPoint(13, 13);
}
