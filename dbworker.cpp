#include "dbworker.h"
#include "handlemesserror.h"
#include <QMessageBox>
#include <QDebug>

dbWorker::dbWorker(QObject *parent) : QObject(parent)
{

}

void dbWorker::ExecQuery(QSqlQuery &query)
{
    bool b=query.exec();
    if (!b)
    {
        QString lastError =QString("Ошибка выполнения запроса: \n\n%1\n\n %2").arg(query.lastQuery()).arg(query.lastError().text());
        throw  handleMessError(lastError,"Работа с БД");

    }
}

QSqlQuery dbWorker::ExecQuery(QString &textSql)
{
    QSqlQuery query;
    query.prepare(textSql);
    ExecQuery(query);
    return query;
}


int dbWorker::getIdPhoto(QString &namePath)
{
    QSqlQuery query;
    QString strSql=QString("select id from photo where name='%1'").arg(namePath);
    query.prepare(strSql);
    ExecQuery(query);
    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        strSql=QString("insert into photo(name) values('%1')").arg(namePath);
        query.prepare(strSql);
        ExecQuery(query);
        return getIdPhoto(namePath);
    }
}



// получ-е типа нарушения: объект вне ИЖС, выходит за границу, не оформленный объект и т.д.
//uint dbWorker::getType(uint ind)
//{

//}

