#ifndef CONTROLPOLYGON_H
#define CONTROLPOLYGON_H

#include <QObject>
#include <QGraphicsPolygonItem>

class controlPolygon:public QGraphicsPolygonItem
{
public:
    controlPolygon(QColor color,const QPolygonF &polygon, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
private:
    QColor color;
};

#endif // CONTROLPOLYGON_H
