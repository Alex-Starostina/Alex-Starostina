#include "formmark.h"
#include "ui_formmark.h"
#include <QSqlQuery>
#include "dbworker.h"
#include "handlemesserror.h"
#include "settings.h"

FormMark::FormMark(MapImageScene *scene_,modeObjectViolation mode_,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMark),
    scene(scene_)
{
    ui->setupUi(this);
    setMode(mode_);

    findedAreaEl1.adr = "";
    findedAreaEl1.kadNum = "";
    findedAreaEl1.price = "";
}


void FormMark::setMode(modeObjectViolation mode)
{
    ui->labelInfoStart->setVisible(false);
    ui->btnCancelDraw->setVisible(false);
    ui->btnDelete->setVisible(false);
    ui->label_KNum->setVisible(false);
    ui->le_KNum->setVisible(false);
    ui->label_adr->setVisible(false);
    ui->te_adr_->setVisible(false);
    this->mode=mode;

    switch(mode)
    {
    case startDraw_:
        ui->labelInfoStart->setVisible(true);
        ui->btnCancelDraw->setVisible(true);
        break;
    default:
        ui->btnDelete->setVisible(true);
        ui->label_KNum->setVisible(true);
        ui->le_KNum->setVisible(true);
        ui->label_adr->setVisible(true);
        ui->te_adr_->setVisible(true);
    }
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);

}

FormMark::~FormMark()
{
    delete ui;
}


 int FormMark::getId()
 {
     return id;
 }

 void FormMark::draw()
 {
     QColor colorCrcl=QColor("red");
     QPen pen;
     QBrush brush(QColor(colorCrcl.red(), colorCrcl.green(), colorCrcl.blue(), 127));

     pen.setWidth(2);
     QRectF rect=QRectF(point.rx()-6, point.ry()-6, 12, 12);
     auto circleFig = scene->addEllipse(rect, pen, brush );
     circleFig->setData(0,id);
     circleFig->setData(1,1);
     circleFig->setZValue(10);
 }

 void FormMark::deleteFromScene()
 {
     QList<QGraphicsItem *> list=scene->items(point);
     foreach(QGraphicsItem *item,list)
     {
         if(item->data(0)==id && item->data(1)==1)
             scene->removeItem(item);
     }
 }


 void FormMark::setPoint(QPoint point_)
 {
     Q_UNUSED(point_)

 }

 void FormMark::save(int idViolation)
 {
     Q_UNUSED(idViolation)
 }

 void FormMark::deleteFromDB()
 {
     dbWorker dbWorker_;
     QString  textSql=QString("delete from  mark  where id=%1").arg(id);
     dbWorker_.ExecQuery(textSql);
 }

//----------------------------------------------------------------------------------------------------------------------------------
 void FormNewMark::setPoint(QPoint point_)
 {
     point=point_;
     draw();
     if(findAreaInfoByPoints())
     {
         ui->le_KNum->setText(findedAreaEl1.kadNum);
         ui->te_adr_->setText(findedAreaEl1.adr);
     }
 }

 bool FormNewMark::findAreaInfoByPoints()
 {

     QList<QVariant> listVariants;
     QTreeWidgetItem *item;

     if (treeWidget->topLevelItemCount() == 0) {
       //showMessage("Не загружено ни одного участка в список слева!");
       return false;
     }
     for(int i=0; i<treeWidget->topLevelItemCount(); i++)
     {
           QPolygon polyg;
           item=treeWidget->topLevelItem(i);
           listVariants = item->data(0,Qt::UserRole).toList();

           for (QVariant el : listVariants) {
             //polyg << scene->toPixel(el.toPointF()).toPoint();
               polyg << el.toPointF().toPoint();
           }

           if (polyg.containsPoint(point, Qt::WindingFill)) {

               findedAreaEl1.adr = item->text(1);
               findedAreaEl1.kadNum = item->text(0);
               findedAreaEl1.price = item->text(3);
               findedAreaEl1.coords=item->data(0, Qt::UserRole).toList();

               findedAreaEl1.ind = i;
               return true;
           }
     }

     return false;
 }

 FormNewMark::FormNewMark(MapImageScene *scene_,QTreeWidget* treeWidget_,QWidget *parent):FormMark(scene_,startDraw_,parent),
     treeWidget(treeWidget_)
 {
     //insertDB();
 }


 void FormNewMark::insertDB()
 {
    dbWorker dbWorker_;
     //textSql="insert into mark(id) values(NULL)";
     //dbWorker_.ExecQuery(textSql);
     QString textSql="select max(id) from  mark";
     QSqlQuery query=dbWorker_.ExecQuery(textSql);
     if(query.next())
     {
        id=query.value(0).toInt();
     }
     else id=1;
 }

 void FormNewMark::save(int idViolation)
 {

     dbWorker dbWorker_;
     QString  textSql=QString("insert into mark(id_violation,x,y,addr,knum) values(%1,%2,%3,'%4','%5')").arg(idViolation).arg(point.x()).arg(point.y())
             .arg(ui->te_adr_->text()).arg(ui->le_KNum->text());
     dbWorker_.ExecQuery(textSql);

 }


void FormNewMark::cancel()
{
    //deleteFromDB();
    deleteFromScene();
}


//------------------------------------
FormDBMark::FormDBMark(MapImageScene *scene_,int id_,QWidget *parent):FormMark(scene_,finishDraw_,parent)
{
    id=id_;
    qDebug()<<"id"<<id;
    initFromDB();
}


void FormDBMark::initFromDB()
{
    QString textSql=QString("select x,y,knum,addr from mark where id=%1").arg(id);
    dbWorker dbWorker_;
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(!query.next())
    {
        qDebug()<<"Не найден идентификатор в таблице mark"<<id;
        return;
    }
    point.setX(query.value("x").toInt());
    point.setY(query.value("y").toInt());

    draw();
    ui->le_KNum->setText(query.value("knum").toString());
    ui->te_adr_->setText(query.value("addr").toString());


}
/*
FormMark_::FormMark_(MapImageScene *scene_,modeObjectViolation mode_,QWidget *parent):formObjectViolation(scene_,mode_,parent)
{
     ui->labelInfoStart->setText("Щелкните мышкой по аэрофотоснимку, чтобы выбрать ориентир");
     typeObject=mark_;
}


void FormMark_::draw()
{
    QColor colorCrcl=QColor("red");
    QPen pen;
    QBrush brush(QColor(colorCrcl.red(), colorCrcl.green(), colorCrcl.blue(), 127));

    pen.setWidth(2);
    QRectF rect=QRectF(points[0].rx()-6, points[0].ry()-6, 12, 12);
    auto circleFig = scene->addEllipse(rect, pen, brush );
    circleFig->setData(0,id);
    circleFig->setData(1,typeObject);

    circleFig->setZValue(10);
    ui->label_info->setText("Участок с кадастровым номером");
}



//----------------------------------------------------------------
FormNewMark_::FormNewMark_(MapImageScene *scene_,QWidget *parent):FormMark_(scene_,startDraw_,parent)
{
    insertDB();

}

void FormNewMark_::insertDB()
{
   dbWorker dbWorker_;
    QString  textSql="insert into mark(id) values(NULL)";
    dbWorker_.ExecQuery(textSql);
    textSql="select max(id) from  mark";
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(query.next())
    {
       id=query.value(0).toInt();
    }
    else id=1;
}

void FormNewMark_::updateDB(int idViolation)
{
    dbWorker dbWorker_;
    QString  textSql=QString("update mark set id_violation=%1,x=%2,y=%3 where id=%4").arg(idViolation).arg(points[0].y()).arg(points[0].x()).arg(id);
    dbWorker_.ExecQuery(textSql);

}

*/

void FormMark::on_btnDelete_clicked()
{
    deleteFromScene();
    emit deleteMark();
}

void FormMark::on_btnCancelDraw_clicked()
{
    scene->setMode(noMode);
    emit deleteMark();
}
