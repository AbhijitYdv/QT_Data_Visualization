
#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QList>

class GraphController : public QObject
{
    Q_OBJECT

public:
    explicit GraphController(QObject *parent = nullptr);

    Q_INVOKABLE void addPoint(double x, double y);
    Q_INVOKABLE void clearGraph();
    Q_INVOKABLE void loadDefaultPoints();

signals:
    void pointAdded(double x, double y);
    void graphCleared();

private:
    QList<QPointF> points;
};

#endif
