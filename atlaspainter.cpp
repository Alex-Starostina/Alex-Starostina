#include "atlaspainter.h"
#include "settings.h"
#include <QDebug>

AtlasPainter::AtlasPainter(QGraphicsScene *scene)
{
    sc = scene;
    widthLine = Settings::Instance().getWidthLine();
}

AtlasPainter::~AtlasPainter()
{
    delete sc;
}


void AtlasPainter::addPoint1(QRectF rect, QColor color = QColor("white"))
{
    QPen pen(color);
    pen.setWidth(3);
    QBrush br(Qt::SolidPattern);
    br.setColor(color);
    sc->addEllipse(rect, pen, br );
}

void AtlasPainter::addText(QString txt, QPointF p, QColor colorTxt, QFont txtFont = QFont("Sans-serif", 16))
{
 //   myFont.setBold(true);
    QGraphicsTextItem *hdrItem = sc->addText(txt, txtFont);
    hdrItem->setPos(p);
    hdrItem->setDefaultTextColor(colorTxt);   
}

QVector<QPoint> AtlasPainter::getAreaPoints()
{
    return areaPoints;

}

QVector<QPoint> AtlasPainter::getLastAreaPoints()
{
    return lastAreaPoints;

}

void AtlasPainter::setMode(TModeScene mode_)
{
    mode=mode_;
    if(areaPoints.count())
    {
    lastAreaPoints=areaPoints;
    }
    areaPoints.clear();
    listSelectedLines.clear();
}


void AtlasPainter::addPoint(QPoint point)
{
    areaPoints.push_back(point);

    if(areaPoints.count()==1) emit setFirstPoint();
    else
    {
        emit getSquare(calculateAreaOnMap(areaPoints));
    }
    if (areaPoints.size() > 1)
    {
        drawLine(areaPoints.at(areaPoints.size() - 1), areaPoints.at(areaPoints.size()-2 ),colorLineSq());
    }
    drawCircle(QRectF(point.rx() - 6, point.ry() - 6, 12, 12), colorLineSq());
}

QString AtlasPainter::colorLineSq()
{
    QString colorLineSq = Settings::Instance().getSquareLineColor();
    if (mode == selectError)
    {
        colorLineSq = Settings::Instance().getErrorLineColor();
    }
    return colorLineSq;
}

void AtlasPainter::drawLine(QPoint p1,QPoint p2,QString colorLineSq)
{
   QGraphicsLineItem *line =sc->addLine(QLineF(p1,p2),
                                         QPen(QBrush(QColor(colorLineSq)), widthLine, Qt::DashLine));
    line->setData(0,0);
    listSelectedLines<<line;
}


void AtlasPainter::drawCircle(QRectF rect, QColor colorCrcl)
{
    QPen pen(colorCrcl);
    QBrush brush(QColor(colorCrcl.red(), colorCrcl.green(), colorCrcl.blue(), 127));

    pen.setWidth(2); //.setWidth(n);
    auto circleFig = sc->addEllipse(rect, pen, brush );
    circleFig->setData(0,0);
    circleFig->setZValue(10);
}


void AtlasPainter::drawLastLine()
{
    if (areaPoints.size() > 1)
    {
        drawLine(areaPoints.at(areaPoints.size() - 1), areaPoints.at(0),colorLineSq());

    }
}

double AtlasPainter::calculateAreaOnMap(QVector<QPoint> points)
{

    QPolygon polyg(points);
    QRect boundRect = polyg.boundingRect();
    QPoint topLeftP = boundRect.topLeft();
    int goriz = topLeftP.rx();
    int maxG = boundRect.topRight().rx()+1;
    int vert = topLeftP.ry();
    int maxV = boundRect.bottomRight().ry()+1;
    QPoint p;
    double sq = boundRect.width() * boundRect.height();

    while (goriz < maxG) {
        for (int i=vert; i<maxV; i++) {
          p.setX(goriz);
          p.setY(i);
          if (!polyg.containsPoint(p, Qt::WindingFill))
              sq--;
        }
      goriz++;
    }

    sq=sq*size_pix.rwidth()*size_pix.rheight();
    return sq;
}



void AtlasPainter::deleteLastPoint()
{
    //удаление послед. отмеч. пользователем точки
    if (areaPoints.size() > 0) {
        QPoint lastP = areaPoints.last();  //последняя-удаляемая точка выделенного участка;

        areaPoints.removeLast();
        auto itemsInPoint = sc->items(lastP);

        if (itemsInPoint.size() > 0) {
            foreach(QGraphicsItem* el, itemsInPoint) {
                if(el->type() == 4) //данный элемент-выделенная цветом большая точка
                    sc->removeItem(el);
            }
        }
     }
    //удаление послед. линии выделенного участка
    if (listSelectedLines.size() > 0) {
        auto lineItem = listSelectedLines.last();
        if (sc->items().contains(lineItem)) {
            sc->removeItem( lineItem);
            listSelectedLines.removeLast();
        }
    }
    if(areaPoints.count()==1) emit setFirstPoint();
    else
    {
        emit getSquare(calculateAreaOnMap(areaPoints));
    }
}

void AtlasPainter::setSizePix(QSizeF size_pix_)
{
    size_pix=size_pix_;
}


float AtlasPainter::meterWidthOfLine(QPoint startP, QPoint endP)
{
    int deltaH = 0, deltaW=0;
    float deltaHMeters, deltaWMeters, len = 0;

    deltaH = abs(startP.y() - endP.y());
    deltaW = abs(startP.x() - endP.x());
    if ((deltaH > 0) | (deltaW > 0)) {
      if (deltaH > 0)  deltaHMeters = deltaH*size_pix.rheight();
      if (deltaW > 0)  deltaWMeters = deltaW * size_pix.rwidth();
      if (deltaH == 0) len = deltaWMeters;
      else {
        if (deltaW == 0) len = deltaHMeters;
        else
          len = sqrt( pow(deltaHMeters, 2) + pow(deltaWMeters, 2));
      }
      len = floor(len*10) / 10;
    }

    return len;
}


