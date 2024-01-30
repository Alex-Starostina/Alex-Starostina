#include "formviolation.h"
#include "ui_formviolation.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <settings.h>
#include "dbworker.h"
#include "handlemesserror.h"


FormViolation::FormViolation(MapImageScene *scene_,TModeScene mode_,QWidget *parent) :
    QWidget(parent),       
    //mode(mode_),
    ui(new Ui::FormViolation),
    formMark(nullptr),
    id(0)
{
    ui->setupUi(this);
    scene=scene_;
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT name,id from type_violation");
    ui->cb_typeBorder->setModel(model);
    connect(scene,SIGNAL(finishDrawBorder_doubleClick()),this,SLOT(setFinishMode()));
    connect(scene,SIGNAL(setFirstPoint()),this,SLOT(setContinueMode()));
    connect(scene,SIGNAL(getSquare(double)),this,SLOT(setSquare(double)));
    connect(scene,SIGNAL(drawMark(QPoint)),this,SLOT(setPointMark(QPoint)));
    //connect(this,SIGNAL(finishDrawSignalViolation()),scene,SLOT(finishDrawBorder()));
    if(mode_ == selectError)
      ui->cb_typeBorder->setCurrentIndex(3);
    getIdBorderFromDb();

}

void FormViolation::getIdBorderFromDb()
{
    dbWorker dbWorker_;
    QString textSql="select max(id) from  border";
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(query.next())
    {
       idCurBorder=query.value(0).toInt();
       idCurBorder++;
    }
    else
        idCurBorder=1;
}

void FormViolation::setFinishMode()
{
    FormBorder_ *border=getLastBorder();
    if(border)
    {
        border->setMode(finishDraw_);       
    }
}

void FormViolation::setPointMark(QPoint p)
{

    formMark->setMode(finishDraw_);
    formMark->setPoint(p);

}

void FormViolation::setContinueMode()
{
    FormBorder_ *border=getLastBorder();
    if(border) border->setMode(continueDraw_);
}


FormViolation::~FormViolation()
{
    delete ui;
}

FormBorder_ *FormViolation::getLastBorder()
{
    int i=ui->tw_Borders->rowCount();
    if(i>0)
    {
        FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i-1,0));
        return border;
    }
    else
        return nullptr;
}

void FormViolation::deleteBorderForm(int data)
{       
   for(int i=0;i<ui->tw_Borders->rowCount();i++)
    {
        if(ui->tw_Borders->item(i,0)->data(Qt::UserRole)==data)
        {
            FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
            ui->tw_Borders->removeRow(i);
            if(!border->isNew()) listDeleted.append(data);
            border->deleteLater();
            return;
        }
    }

}

void FormViolation::setSquare(double sq)
{
    FormBorder_ *border=getLastBorder();
    if(border) border->setSquare(sq);
}

int FormViolation::addNewBorder(TModeScene mode_)
{
   int i=ui->tw_Borders->rowCount();

    FormBorder_ *border=getLastBorder();

    if(border)
    {
        if(border->getMode()==startDraw_)
        {
            QMessageBox::warning(0,
                                 Settings::Instance().getAppName(),
                                 "Щелкните мышкой по аэрофотоснимку, чтобы указать место нарушения",
                                 QMessageBox::Ok);
            return 0;
        }
        if(border->getMode()==continueDraw_)
        {

            //завершить нанесение
            if(QMessageBox::Yes == QMessageBox::information(0,
                                                            Settings::Instance().getAppName(),
                                                            "Завершить нанесение?",
                                                            QMessageBox::Yes|QMessageBox::No))
            {
                border->setMode(finishDraw_);
            }
            else return 0;
        }
    }
    borderObj *obj=new newBorderObj(scene,mode_);
    obj->id=idCurBorder;
    obj->typeBorder=mode_;
    obj->id_violation=id;
    border=new FormBorder_(obj,startDraw_);
    idCurBorder=idCurBorder+1;

    connect(border,SIGNAL(deleteForm(int)),this,SLOT(deleteBorderForm(int)));
    connect(border,SIGNAL(finishDrawSignal()),scene,SLOT(finishDrawBorder()));
    QTableWidgetItem *cur = new QTableWidgetItem();

    cur->setData(Qt::UserRole,border->getId());
    ui->tw_Borders->setRowCount(i+1);
    ui->tw_Borders->setItem(i,0,cur);

    ui->tw_Borders->setCellWidget(i,0, border);
    ui->tw_Borders->setRowHeight(i, border->sizeHint().height());

    return idCurBorder-1;
}


int FormViolation::addNewMark(QTreeWidget *treeWidget)
{
    if (formMark!=nullptr)
    {
        throw  handleMessError("Для нарушения уже указан ориентир","Указание ориентира");
    }

    formMark=new FormNewMark(scene,treeWidget);
    connect(formMark,SIGNAL(deleteMark()),this,SLOT(deleteMarkViolation()));

    QBoxLayout* layout=static_cast<QBoxLayout*>(this->layout());
    layout->insertWidget(2,formMark);


    return formMark->getId();
}


void FormViolation::deleteMarkViolation()
{
    formMark->deleteLater();
    formMark=nullptr;
}





void FormViolation::on_btnCancel_clicked()
{

    try
    {
        for(int i=ui->tw_Borders->rowCount()-1;i>=0;i--)
        {
            FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
            if(border->isNew())
            border->cancel();
        }
        if(formMark!=nullptr) formMark->cancel();
        clear();
        foreach(int id,listDeleted)
        {
            dbBorderObj *border=new dbBorderObj(scene,id);
            border->draw();
        }


    }
    catch (handleMessError &e)
    {

        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }

    //clear();
    //удал-е отметок площади и длин сторон
    scene->clearMarkupOnPhoto();
}


void FormViolation::clear(bool refresh)
{
   for(int i=ui->tw_Borders->rowCount()-1;i>0;i--)
    {
        FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
        border->deleteLater();
    }
    ui->tw_Borders->clear();
    ui->tw_Borders->setRowCount(0);
    if (formMark!=nullptr) formMark->deleteFromScene();
    emit closeViolation(refresh);
    close();
}


QPointF FormViolation::centerMap()
{
    QPointF p;
    return p;
}

//--------------------------------------------------------------------------------
FormNewViolation::FormNewViolation(MapImageScene *scene_,TModeScene mode_,QWidget *parent):FormViolation(scene_,mode_,parent)
{
}



int FormNewViolation::insertDB()
{
    dbWorker dbWorker_;
    QModelIndex index=ui->cb_typeBorder->model()->index(ui->cb_typeBorder->currentIndex(),1);
    int idType=ui->cb_typeBorder->model()->data(index).toInt();
    QString  textSql=QString("insert into violation(id_photo,id_type) values(%1,%2)")
            .arg(Settings::Instance().getIdPhoto()).arg(idType);
    dbWorker_.ExecQuery(textSql);
    textSql="select max(id) from violation";
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    if(query.next())
        return(query.value(0).toInt());
    else
        throw  handleMessError("Не удалось получить идентификатора нарушения","Чтение данных из БД");
}

void FormNewViolation::on_btnSave_clicked()
{
    FormBorder_ *border=getLastBorder();
    if(!border) return;

    if(border->getMode()==startDraw_ ||border->getMode()==continueDraw_)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             "Завершите нанесение",
                             QMessageBox::Ok);
        return;
    }
    try
    {
        //создать запись в таблице violation
        id=insertDB();
        //обновить данные для всех border
        for(int i=0;i<ui->tw_Borders->rowCount();i++)
        {
            FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
            border->save(id);
        }
        if(formMark!=nullptr)
        formMark->save(id);
        clear(true);
        //emit insertViolationDB();
        //emit closeViolation(refresh);
    }
    catch (handleMessError &e)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
}


//--------------------------------------------------------------------

FormDBViolation::FormDBViolation(MapImageScene *scene_,TModeScene mode_,int id_,int typeV,QWidget *parent):FormViolation(scene_,mode_,parent)
{
    id=id_;
    existMark=false;
    init();
    qDebug()<<typeV;
    ui->cb_typeBorder->setCurrentIndex(typeV);
}

void FormDBViolation::init()
{
    if (id==0) return;
    dbWorker dbWorker_;
    QString textSql=QString("select id from  border where id_violation=%1").arg(id);
    //qDebug()<<"textSql";
    dbWorker_.ExecQuery(textSql);
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    while(query.next())
    {
        addBorder(query.value(0).toInt());
    }
    addDBMark();
}

void FormDBViolation::addBorder(int idBorder)
{
    int i=ui->tw_Borders->rowCount();
    dbBorderObj *obj = new dbBorderObj(scene,idBorder);
    FormBorder_ *border=new FormBorder_(obj,None);
    connect(border,SIGNAL(deleteForm(int)),this,SLOT(deleteBorderForm(int)));
    connect(border,SIGNAL(finishDrawSignal()),scene,SLOT(finishDrawBorder()));
    QTableWidgetItem *cur = new QTableWidgetItem();
    cur->setData(Qt::UserRole,border->getId());
    ui->tw_Borders->setRowCount(i+1);
    ui->tw_Borders->setItem(i,0,cur);
    ui->tw_Borders->setCellWidget(i,0, border);
    ui->tw_Borders->setRowHeight(i,border->sizeHint().height());
}

void FormDBViolation::addDBMark()
{
    if (id==0) return;
    dbWorker dbWorker_;
    QString textSql=QString("select id from  mark where id_violation=%1").arg(id);
    dbWorker_.ExecQuery(textSql);
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    while(query.next())
    {
        formMark=new FormDBMark(scene,query.value(0).toInt());
        connect(formMark,SIGNAL(deleteMark()),this,SLOT(deleteMarkViolation()));

        QBoxLayout* layout=static_cast<QBoxLayout*>(this->layout());
        layout->insertWidget(2,formMark);
        idMark=query.value(0).toInt();
    }
}

QPointF FormDBViolation::centerMap()
{
    if(formMark!=nullptr)
      return formMark->point;
    if(ui->tw_Borders->rowCount()>0)
    {
        FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(0,0));
        return border->center();
    }
    return QPointF();
}


void FormDBViolation::on_btnSave_clicked()
{   
    FormBorder_ *border=getLastBorder();
    if(border->getMode()==startDraw_ ||border->getMode()==continueDraw_)

    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             "Завершите нанесение",
                             QMessageBox::Ok);
        return;
    }
    try
    {
        //обновить тип нарушения
        dbWorker dbWorker_;
        QModelIndex index=ui->cb_typeBorder->model()->index(ui->cb_typeBorder->currentIndex(),1);
        int idType=ui->cb_typeBorder->model()->data(index).toInt();
        QString  textSql=QString("update  violation set id_type=%1 where id= %2")
                .arg(idType).arg(id);
        dbWorker_.ExecQuery(textSql);
        //обновить данные для всех border
        for(int i=0;i<ui->tw_Borders->rowCount();i++)
        {
            FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
            if(border->isNew())
            {
                qDebug()<<"border->isNew()";
                 border->save(id);
            }

        }
        if((formMark==nullptr or formMark->isNew()) and idMark>0)
        {
            dbWorker dbWorker_;
            QString  textSql=QString("delete from  mark where id=%1").arg(idMark);
            dbWorker_.ExecQuery(textSql);

        }
        if(formMark!=nullptr and formMark->isNew())
        formMark->save(id);

        foreach(int idBorder,listDeleted)
        {
            dbWorker dbWorker_;
            QString  textSql=QString("delete from  point_border where id_border=%1").arg(idBorder);
            dbWorker_.ExecQuery(textSql);
            textSql=QString("delete from  border where id=%1").arg(idBorder);
            dbWorker_.ExecQuery(textSql);
        }
        clear(true);

    }
    catch (handleMessError &e)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
}

bool FormDBViolation::isModified()
{
    // есть ли новые объекты
    for(int i=0;i<ui->tw_Borders->rowCount();i++)
    {
        FormBorder_ *border=static_cast<FormBorder_*>(ui->tw_Borders->cellWidget(i,0));
        if(border->isNew()) return true;
    }
    if (formMark!=nullptr && formMark->isNew()) return true;

    //есть ли удаленные объекты
    return (listDeleted.count()>0);
}



/*void FormDBViolation::on_btnCancel_clicked()
{

}*/






void FormViolation::on_btnSave_clicked()
{

}
