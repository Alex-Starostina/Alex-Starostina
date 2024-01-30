#include "borderobj.h"
#include <QSqlQuery>
#include "dbworker.h"
#include "handlemesserror.h"
#include "settings.h"

borderObj::borderObj(MapImageScene *scene_,QObject *parent) : QObject(parent)
  ,scene(scene_)
{

}


QString borderObj::colorLineSq()
{
    QString colorLineSq = Settings::Instance().getSquareLineColor();
    if (typeBorder == 2)
    {
        colorLineSq = Settings::Instance().getErrorLineColor();
    }
    return colorLineSq;
}

void borderObj::draw()
{
    QPolygon polygon(points);
    int widthLine = Settings::Instance().getWidthLine();
    QPen pen(colorLineSq());
    pen.setWidth(widthLine);
    QGraphicsPolygonItem *item=scene->addPolygon(polygon,pen);
    item->setData(0, id);
    item->setData(1,0);
}

void borderObj::cancelDraw()
{
    //scene->deleteLastBorder();
    deleteTempBorder();
    scene->setMode(noMode);
}

void borderObj::deleteTempBorder()
{
    foreach(QPoint point,points)
    {
        QList<QGraphicsItem *> list=scene->items(point);
        foreach(QGraphicsItem *item,list)
        {
            if(item->data(0)==0)
            scene->removeItem(item);
        }
    }
}

void borderObj::deleteFromScene()
{
    foreach(QPoint point,points)
    {
        QList<QGraphicsItem *> list=scene->items(point);
        foreach(QGraphicsItem *item,list)
        {
            if(item->data(0)==id and item->data(1)==0)
            scene->removeItem(item);
        }
    }
    QPolygon polygon(points);
}

void borderObj::setPoints(QVector <QPoint> points_)
{
    points=points_;
    deleteFromScene();
    draw();
}

void borderObj::deleteDB()
{
    dbWorker dbWorker_;
    QString  textSql=QString("delete from  border where id=%1").arg(id);
    dbWorker_.ExecQuery(textSql);
    textSql=QString("delete from  point_border where id_border=%1").arg(id);
    dbWorker_.ExecQuery(textSql);
}


//-------------------------------------------------------------
newBorderObj::newBorderObj(MapImageScene *scene_,int type_,QObject *parent):borderObj(scene_,parent)
{
    typeBorder=type_;
}

void newBorderObj::save(int idViolation)
{
    dbWorker dbWorker_;
//    id_violation=idViolation;
    QString  textSql=QString("insert into border(id,id_violation,id_type,square) values(%1,%2,%3,%4)")
                             .arg(id).arg(idViolation).arg(typeBorder).arg(square);
    dbWorker_.ExecQuery(textSql);
    for(int i=0;i<points.count();i++)
    {
        textSql=QString("insert into point_border(id_border,num,x,y) values(%1,%2,%3,%4)")
                .arg(id).arg(i+1).arg(points.at(i).x()).arg(points.at(i).y());
        QSqlQuery query=dbWorker_.ExecQuery(textSql);
    }
}

void newBorderObj::cancel()
{
    deleteFromScene();
    deleteDB();
}

void newBorderObj::finishDraw()
{
    points=scene->getPointsOfBorder();
    //scene->deleteLastBorder();
    deleteTempBorder();
    draw();
}
//-------------------------------------------------------------
dbBorderObj::dbBorderObj(MapImageScene *scene_,int id_,QObject *parent):borderObj(scene_,parent)
{
    id=id_;
    initFromDB();
}

void dbBorderObj::initFromDB()
{
    QString textSql=QString("select id,id_violation,id_type,square from border where id=%1").arg(id);
    dbWorker dbWorker_;
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(!query.next())
    {
        qDebug()<<"Не найден идентификатор в таблице border";
        return;
    }
    id=query.value(0).toInt();
    id_violation=query.value(1).toInt();
    typeBorder=query.value(2).toInt();
    square=query.value(3).toInt();
    textSql=QString("select x,y from point_border where id_border=%1 order by num").arg(id);
    query=dbWorker_.ExecQuery(textSql);
    while(query.next())
    {
        QPoint p(query.value(0).toInt(),query.value(1).toInt());
        points<<p;
    }
}

void dbBorderObj::cancel()
{
    draw();
}

/*void dbBorderObj::finishDraw()
{
    draw();
}*/



