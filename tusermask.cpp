#include "tusermask.h"
#include <QDebug>
#include <QPainter>
#include <QBitmap>
#include <settings.h>
#include <dbworker.h>

TUserMask::TUserMask()
{

}

void TUserMask::setStartPixMap(QPixmap source)
{
    isStart=true;
    addPixMap(source);
    createListPolygons();
}

void TUserMask::addPixMap(QPixmap source)
{    
    if(pixmap.isNull()|| isStart==true)
    {
        pixmap=source;
        isStart=false;
    }
    else
    {
        QPainter painter(&pixmap);
        painter.drawPixmap(pixmap.rect(),source,source.rect());
        painter.end();
    }
    int i=pixmap.width()/2;
    int j=pixmap.height()/2;
    pos0=QPoint(i,j);
    pos=pos0;
    pixmap_start=pixmap;
}

QPoint TUserMask::getPointForView()
{
    QImage img=pixmap.toImage();
    while(!(img.pixelColor(pos)==Qt::red) || containsPoint(pos))
    {
        pos.rx()++;

        if(pos.rx()>=img.width())
        {
            pos.setX(0);
            pos.ry()++;;
        }
        if(pos.ry()>=img.height())
        {
           pos=QPoint(0,0);
        }
        if(pos==pos0)
            return QPoint(-1,-1);
    }

    listPos<<pos;
    return pos;
}

QPoint TUserMask::getPointForViewPrev()
{
    if(listPos.count()>1)
    {
    listPos.removeLast();
    return listPos.last();
    }
    else return QPoint(-1,-1);
}

void TUserMask::clearPolygon(QPolygon &polygon)
{    
     /*QImage img=pixmap.toImage();
     QPainter painter(&pixmap);
     painter.setBrush(QColor(0,0,0));
     painter.setPen(QColor(0,0,0));
     painter.drawPolygon(polygon);
     painter.end();*/
}


void TUserMask::revertPolygon(QPolygon &polygon)
{
    /*qDebug()<<"revertPolygon";
     QImage img=pixmap.toImage();
     QImage img_start=pixmap_start.toImage();

     QPainter painter(&pixmap);
     QRect rect=polygon.boundingRect();
     for(int i=rect.topLeft().x();i<rect.bottomRight().x();i++)
        for(int j=rect.topLeft().y();j<rect.bottomRight().y();j++)
        {
            QPoint p(i,j);
            if (polygon.containsPoint(p,Qt::OddEvenFill))
            {
                img.setPixel(i,j,img_start.pixel(i,j));
            }
        }

     pixmap.fromImage(img);*/
}


void TUserMask::createListPolygons()
{
    listPolygon.clear();
    QString textSql=QString("select * from border where  id_violation in (select id from violation where id_photo=%1)")
            .arg(Settings::Instance().getIdPhoto());
    dbWorker dbWorker_;

    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    while(query.next())
    {
        textSql=QString("select * from point_border where id_border=%1 order by num").arg(query.value("id").toInt());
        QSqlQuery query_point=dbWorker_.ExecQuery(textSql);
        QVector <QPoint> points;

        while(query_point.next())
        {
            QPoint p(query_point.value("x").toInt(),query_point.value("y").toInt());
            points<<p;

        }
        if (points.count() > 0)
        {
            listPolygon.append(QPolygon(points));
        }

    }
}

bool TUserMask::containsPoint(const QPoint &point)
{
    for(int i=0; i<listPolygon.count();i++)
    {
        if (listPolygon[i].containsPoint(point,Qt::OddEvenFill)) return true;
    }
    return false;
}
