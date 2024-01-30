#include "controlpolygon.h"

#include<QBrush>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include "settings.h"

controlPolygon::controlPolygon(QColor color, const QPolygonF &polygon, QGraphicsItem *parent)
    :QGraphicsPolygonItem(polygon,parent)
{
     setFlags(ItemIsSelectable);
     this->color=color;
}

void controlPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

   QColor fillColor = (option->state & QStyle::State_Selected) ? Qt::white : color;
    QPen oldPen = painter->pen();
    QPen pen(fillColor);
    pen.setWidth(Settings::Instance().getWidthLine());
    painter->setPen(pen);
    //painter->setBrush(QBrush(Qt::blue));
    //painter->setBrush(isSelected() ? QBrush(Qt::gray) : Qt::NoBrush);
    QPen pen1=painter->pen();
    painter->drawPolygon(this->polygon());
    painter->setPen(oldPen);
}


