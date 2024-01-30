#include "pixelpolygon.h"
#include <QDebug>
#include <math.h>

PixelPolygon::PixelPolygon(QImage *mask_):mask(mask_)
{

}

qreal length(QLine line)
{
    double x2=(line.p1().x()-line.p2().x());
    double y2=(line.p1().y()-line.p1().y());
    return(sqrt(x2*x2+y2*y2));
}

QPolygonF PixelPolygon::findNewPolygon()
{
     QPolygonF polygonUp;
     QPolygonF polygonDown;
     QLine lineV=findStartLine();
     polygonUp<<lineV.p1();
     polygonDown<<lineV.p2();
     int p=0;
     while(lineV.p1().x()+1<mask->width())
     {
        p++;
        //if (p>20) return polygon;

        lineV.setP1(findNextUpPoint(lineV));
        if(lineV.p1().isNull()) return createPolygon(polygonUp,polygonDown);

        lineV.setP2(findNextDownPoint(lineV));
        if(lineV.p2().isNull()) return createPolygon(polygonUp,polygonDown);
        polygonUp<<lineV.p1();
        polygonDown<<lineV.p2();
    }


    return createPolygon(polygonUp,polygonDown);
}


QPolygonF PixelPolygon::createPolygon(const QPolygonF &polygonUp,const QPolygonF &polygonDown)
{
    QPolygonF polygon(polygonUp);
    for(int i=polygonDown.count();i>0;i--)
    polygon<<polygonDown.at(i-1);
    return polygon;
}

QLine PixelPolygon::findStartLine()
{
    QLine lineV;
    for(int i=0;i<mask->width();i++)
    {
        bool isBegin=false;
        for(int j=0;j<mask->height();j++)
        {
            if(mask->valid(i,j))
            {
                if(mask->pixelColor(i,j)==colorWhite)
                {

                    if(!isBegin) lineV.setP1(QPoint(i,j));
                    isBegin=true;
                }
                else
                {
                    if(isBegin)
                    {
                        lineV.setP2(QPoint(i,j-1));
                        return lineV;
                    }
                }
            }
        }
    }
    return lineV;
}

QPoint PixelPolygon::findNextUpPoint(QLine line)
{
    int x=line.p1().x()+1;
    int y=line.p1().y();
    if(!mask->valid(x,y))return QPoint(0,0);
    if(mask->pixelColor(x,y)==colorWhite)
    {
        while(mask->valid(x,y) && mask->pixelColor(x,y)==colorWhite&& y>=0)
        {
           y--;
        }
        return QPoint(x,y+1);

    }
    else
    {
        while(mask->valid(x,y) && mask->pixelColor(x,y)!=colorWhite && y<=line.p2().y())
        {
            y++;
        }
        if(mask->pixelColor(x,y)==colorWhite)
        return QPoint(x,y);
    }
    return QPoint(0,0);
}

QPoint PixelPolygon::findNextDownPoint(QLine line)
{
    int x=line.p2().x()+1;
    int y=line.p2().y();
    if(!mask->valid(x,y))return QPoint(0,0);
    if(mask->pixelColor(x,y)==colorWhite)
    {
        while(mask->valid(x,y) && mask->pixelColor(x,y)==colorWhite&&y<mask->height())
        y++;
        return QPoint(x,y);
    }
    else
    {
        while(mask->valid(x,y) && mask->pixelColor(x,y)!=colorWhite && y>=line.p1().y())
        y--;
        if(mask->pixelColor(x,y)==colorWhite) return QPoint(x,y+1);
    }

    //if(mask->pixelColor(x,y)==colorWhite)
    return QPoint(0,0);
    //else QPoint(0,0);
}

QPolygonF PixelPolygon::AlgorithmBeatle()
{
    int X,Y;//Координаты первой встречи с объектом
    TDirectional Direct; // Направление движения жука
    QColor B;// Значение текущего пиксела
    int cX,cY; // Текущие координаты маркера
    QPolygonF polygon;
    // Идем до тех пор, пока не встретим черную область
    for (Y = 0; Y < mask->height(); Y++)
    {
        //
        for (X = 0; X < mask->width(); X++)
        {
            B =mask->pixelColor(X,Y);
            if (B==colorWhite) break;
        } // Если встречен объект, отличающийся от цвета фона (255 - белый)
         // прервать поиск
         if (X != mask->width()) break;
    }

    // Если не нашли ни одного черного пиксела, то выходим из процедуры
    if ((X == mask->width()) && (mask->height())) return polygon;

    // Если все нормально, начинаем обход по алгоритму жука
    //ToLine = (Byte*)ToImage->ScanLine[Y];
    //ToLine[X] = 0;
    polygon.append(QPointF(X,Y));
    // Поворачиваем налево (новое направление - север)
    cX = X;
    cY = Y - 1;
    Direct = North;


    //Line = (Byte*)FromImage->ScanLine[cY]; // Пока не придем в исходную точку, выделяем контур объекта
    while ((cX != X) || (cY != Y))
    {
        //if (polygon.count()>100) return polygon;
        if (mask->valid(cX,cY))
            B = mask->pixelColor(cX,cY);
        else
            B=colorWhite+1;

        // В зависимости от текущего направления движения жука
        switch (Direct)
        {
        // Север
        case North:
        {
            // Если элемент "черный", поворачиваем снова "налево"
            if (B==colorWhite)
            {

                Direct = West; cX--;
                polygon.append(QPointF(cX,cY));
            }
            // Иначе поворачиваем "направо"
            else {

                Direct = East;
                cX++;
            }
        }
            break;
        // Восток
        case East:
        {
            // Если элемент "черный", поворачиваем снова "налево"
            if (B==colorWhite)
            {
                Direct = North;
                cY--;
                //qDebug()<<cX<<cY;
                polygon.append(QPointF(cX,cY));
            }
            // Иначе поворачиваем "направо"
            else
            {
                Direct = South;
                cY++;

            }
        }
            break;

            // Юг
            case South:
            {
            if (B==colorWhite)
             {

                Direct = East;
                cX++;
                //qDebug()<<cX<<cY;
                polygon.append(QPointF(cX,cY));
            }
            // Иначе поворачиваем "направо"
            else { Direct = West; cX--; }
            }
            break;
            // Запад
            case West:
             {
            if (B==colorWhite)
             {

                Direct = South;
                cY++;
                //qDebug()<<cX<<cY;
                polygon.append(QPointF(cX,cY));
            }
            // Иначе поворачиваем "направо"
            else
             { Direct = North;
                cY--;

              }
            }
        }

  }
    return polygon;
}

