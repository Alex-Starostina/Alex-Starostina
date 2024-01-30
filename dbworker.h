#ifndef DBWORKER_H
#define DBWORKER_H

#include <QObject>
#include <QtSql>

class BorderInfo: public QObject
{
    Q_OBJECT
public:
    int id;
    int id_violation;
    int type;
//    QVector <QPoint> &points;

};

class dbWorker : public QObject
{

public:
    explicit dbWorker(QObject *parent = nullptr);
    int getIdPhoto(QString &namePath);
    void ExecQuery(QSqlQuery &query);
    QSqlQuery ExecQuery(QString &textSql);



signals:

};

#endif // DBWORKER_H
