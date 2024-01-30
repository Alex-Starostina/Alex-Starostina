#ifndef BORDEROBJ_H
#define BORDEROBJ_H

#include <QObject>
#include "mapimagescene.h"

class borderObj : public QObject
{
    Q_OBJECT

public:
    borderObj(MapImageScene *scene_,QObject *parent = nullptr);
    void draw();
    void setPoints(QVector <QPoint> points_);
    void deleteFromScene();
    //int getId();
    //int getSq();
    //void setSq(int sq);
    int square;
    virtual void save(int idViolation){}
    void deleteDB();
    virtual void cancel(){}
    void cancelDraw();
    virtual void finishDraw(){}
    virtual bool isNew(){return false;}
    int typeBorder;
    int id;
    int id_violation;
    QVector <QPoint> points;
protected:    

    void deleteTempBorder();
    MapImageScene *scene;
    QString colorLineSq();
};



class newBorderObj : public borderObj
{
    Q_OBJECT
public:
    newBorderObj(MapImageScene *scene_,int type_,QObject *parent = nullptr);
    void cancel();
    void save(int idViolation);
    void finishDraw();
     bool isNew(){return true;}



};


class dbBorderObj : public borderObj
{
    Q_OBJECT
public:
    dbBorderObj(MapImageScene *scene_, int id_, QObject *parent = nullptr);
    void cancel();
    //void finishDraw();
private:
    void initFromDB();


};




#endif // BORDEROBJ_H
