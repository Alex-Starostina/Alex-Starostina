#include "formobjectviolation.h"
#include "ui_formobjectviolation.h"
#include <QSqlQuery>
#include "dbworker.h"
#include "handlemesserror.h"
#include "settings.h"

FormBorder_::FormBorder_(borderObj *obj_,modeObjectViolation mode_,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::formObjectViolation),
    obj(obj_)
{
    ui->setupUi(this);
    setMode(mode_);
    showInfo();
}

void FormBorder_::showInfo()
{
    ui->labelName->setText("Граница");
    if(obj->typeBorder==2) ui->labelName->setText("Ложное срабатывание");
    setSquare(obj->square);
}

FormBorder_::~FormBorder_()
{
    delete ui;
}

void FormBorder_::setMode(modeObjectViolation mode)
{
    ui->btnCancelDraw->setVisible(false);
    ui->btnFinishDraw->setVisible(false);
    ui->btnDeleteBorder->setVisible(false);
    ui->label_info->setVisible(false);
    //ui->cb_typeBorder->setVisible(false);
    //ui->label_typeBorder->setVisible(false);
    //ui->le_sqBorder->setVisible(false);
    ui->labelInfoStart->setVisible(false);
    this->mode=mode;
    switch(mode)
    {
    case startDraw_:
        ui->labelInfoStart->setVisible(true);
        ui->labelInfoStart->setText("Щелкните мышкой по аэрофотоснимку, чтобы указать место нарушения");
        ui->btnCancelDraw->setVisible(true);
        break;
    case continueDraw_:
        ui->btnCancelDraw->setVisible(true);
        ui->btnFinishDraw->setVisible(true);
        ui->label_info->setVisible(true);       
        break;
    case finishDraw_:
        ui->btnDeleteBorder->setVisible(true);
        ui->label_info->setVisible(true);
        obj->finishDraw();
        break;
    default:
        ui->btnDeleteBorder->setVisible(true);
        ui->label_info->setVisible(true);
    }
}

modeObjectViolation FormBorder_::getMode()
{
    return mode;
}

int FormBorder_::getId()
{
    return obj->id;
}


void FormBorder_::setPoints(QVector <QPoint> points_)
{
    obj->setPoints(points_);
}


void FormBorder_::on_btnCancelDraw_clicked()
{    
    obj->cancelDraw();
    emit deleteForm(obj->id);
}

void FormBorder_::on_btnFinishDraw_clicked()
{
    setMode(finishDraw_);
    emit finishDrawSignal();   
}

void FormBorder_::on_btnDeleteBorder_clicked()
{
    obj->deleteFromScene();
    emit deleteForm(obj->id);
}

void FormBorder_::setSquare(double sq)
{
    obj->square=sq;
    ui->label_info->setText(QString("Площадь %1 кв.м").arg(int(sq)));
}


QString FormBorder_::colorLineSq()
{
    QString colorLineSq = Settings::Instance().getSquareLineColor();
    if (obj->typeBorder == 2)
    {
        colorLineSq = Settings::Instance().getErrorLineColor();
    }
    return colorLineSq;
}

 QPointF  FormBorder_::center()
 {
     return obj->points[0];
 }

/*void FormBorder_::draw()
{
    QPolygon polygon(obj->points);
    int widthLine = Settings::Instance().getWidthLine();

   // QPen pen(colorLineSq());
   //устран-е ошибки "QPainter::setPen: Painter not active"

    QColor colr(colorLineSq());
    QPen pen( colr);

    pen.setWidth(widthLine);
    QGraphicsPolygonItem *item=scene->addPolygon(polygon, pen);
    item->setData(0, obj->id);
    item->setData(1,0);
    scene->clearPolygon(polygon);


//    QPainter pntr(scene );
//    pntr.setBrush(QColor(0,0,0));
//    pntr.setPen(QColor(0,0,0));
//    pntr.drawPolygon(polygon);
//    pntr.end();

}

void FormBorder_::deleteDB()
{
    dbWorker dbWorker_;
    QString  textSql=QString("delete from  border where id=%1").arg(obj->id);
    dbWorker_.ExecQuery(textSql);
}*/

void FormBorder_::save(int idViolation)
{
    /*dbWorker dbWorker_;
    QString  textSql=QString("insert into border(id,id_violation,id_type,square) values(%1,%2,%3,%4)")
                             .arg(obj->id).arg(id_violation).arg(obj->type).arg(obj->square);
    qDebug()<<textSql;
    dbWorker_.ExecQuery(textSql);
    for(int i=0;i<obj->points.count();i++)
    {
        textSql=QString("insert into point_border(id_border,num,x,y) values(%1,%2,%3,%4)")
                .arg(obj->id).arg(i+1).arg(obj->points.at(i).x()).arg(obj->points.at(i).y());
        QSqlQuery query=dbWorker_.ExecQuery(textSql);
    }*/
    obj->save(idViolation);

}

void FormBorder_::cancel()
{
    obj->deleteFromScene();
}
//--------------------------------------------------------------------------------------------------------------
/*FormNewBorder_::FormNewBorder_(borderObj *obj_,QWidget *parent):FormBorder_(obj_,startDraw_,parent)
{
    printType();
}






//-------------------------------------------------------------
FormDBBorder_::FormDBBorder_(borderObj *obj_,QWidget *parent):FormBorder_(obj_,finishDraw_,parent)
{
     initFromDB();
      setSquare(obj->square);
      printType();
}

void FormDBBorder_::initFromDB()
{
    QString textSql=QString("select id,id_violation,id_type,square from border where id=%1").arg(obj->id);
    dbWorker dbWorker_;
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(!query.next())
    {
        qDebug()<<"Не найден идентификатор в таблице border";
        return;
    }
    obj->id=query.value(0).toInt();
    //id_violation=query.value(1).toInt();
    obj->type=query.value(2).toInt();
    obj->square=query.value(3).toInt();
    textSql=QString("select x,y from point_border where id_border=%1 order by num").arg(obj->id);
    query=dbWorker_.ExecQuery(textSql);
    while(query.next())
    {
        QPoint p(query.value(0).toInt(),query.value(1).toInt());
        obj->points<<p;
    }
}
*/


//-----------------------------------------




