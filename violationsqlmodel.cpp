#include "violationsqlmodel.h"
#include <settings.h>
#include <dbworker.h>
#include <QMessageBox>
#include "handlemesserror.h"
#include <QDebug>

ViolationSqlModel::ViolationSqlModel(QObject *parent) : QSqlQueryModel(parent)
{


}

void ViolationSqlModel::init()
{
     QString textSql=QString("select  violation.id,knum,addr,type_violation.name,id_type from violation left outer join mark on mark.id_violation=violation.id,"
                             "type_violation where type_violation.id=id_type and id_photo=%1")
            .arg(Settings::Instance().getIdPhoto());
    //qDebug()<<textSql;
    setQuery(textSql);
    setHeaderData(0, Qt::Horizontal, "N");
    setHeaderData(1, Qt::Horizontal, "Кад номер");
    setHeaderData(2, Qt::Horizontal, "Адрес");
    setHeaderData(3, Qt::Horizontal, "Тип");

}

int ViolationSqlModel::getId(const QModelIndex &ind)
{
    int row=ind.row();
    QModelIndex indexId=index(row,0);
    return data(indexId).toInt();
}


int ViolationSqlModel::getTypeViolation(const QModelIndex &ind)
{
    int row=ind.row();
    QModelIndex indexId=index(row,4);
    return data(indexId).toInt()-1;
}


QPointF ViolationSqlModel::getCenter(const QModelIndex &ind)
{
    int id=getId(ind);
    dbWorker dbWorker_;
    QString textSql=QString("select * from  mark where id_violation=%1").arg(id);
    QSqlQuery query =dbWorker_.ExecQuery(textSql);
    if(query.next())
    {
        return QPointF(query.value("x").toInt(),query.value("y").toInt());
    }

    textSql=QString("select x,y from point_border where id_border in (select  id from border where id_violation=%1) order by num").arg(id);
    query =dbWorker_.ExecQuery(textSql);
    if(query.next())
    {
        return QPointF(query.value("x").toInt(),query.value("y").toInt());
    }
    return QPointF();
}


void ViolationSqlModel::deleteFromModel(QModelIndex &ind)
{
    try {
        dbWorker dbWorker_;
        int id_violation=getId(ind);        
        QString  textSql=QString("select id from border where id_violation=%1").arg(id_violation);
        QSqlQuery query =dbWorker_.ExecQuery(textSql);
        while(query.next())
        {
            emit deleteBorderFromModel(query.value(0).toInt());
        }
        textSql=QString("delete from violation where id=%1").arg(id_violation);
        dbWorker_.ExecQuery(textSql);
        init();
    } catch (handleMessError &e)
    {

        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
}



