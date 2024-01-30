#ifndef FORMOBJECTVIOLATION_H
#define FORMOBJECTVIOLATION_H

#include <QWidget>
#include "mapimagescene.h"
#include "borderobj.h"
namespace Ui {
class formObjectViolation;
}



class FormBorder_ : public QWidget
{
    Q_OBJECT

public:
    FormBorder_(borderObj *obj_,modeObjectViolation mode_,QWidget *parent = nullptr);
    ~FormBorder_();
    void setMode(modeObjectViolation mode_);
    void setPoints(QVector <QPoint> points_);
    modeObjectViolation getMode();
    int getId();
    void save(int idViolation);
    void setSquare(double sq);
    bool isNew(){return obj->isNew();}
    void cancel();
    QPointF center();
protected:
    Ui::formObjectViolation *ui;
    modeObjectViolation mode;
    borderObj *obj;
    //virtual void deleteDB();
    void showInfo();
    QString colorLineSq();

protected slots:
    void on_btnCancelDraw_clicked();
    void on_btnFinishDraw_clicked();
    void on_btnDeleteBorder_clicked();


signals:
    void finishDrawSignal();
    void deleteForm(int id);
};


/*class FormNewBorder_ : public FormBorder_
{
    Q_OBJECT
public:
    FormNewBorder_(borderObj *obj_,QWidget *parent = nullptr);
    void cancel(); 
    bool isNew(){return true;}

};


class FormDBBorder_ : public FormBorder_
{
    Q_OBJECT
public:
    FormDBBorder_(borderObj *obj_,QWidget *parent = nullptr);
private:
    void initFromDB();
};*/



#endif // FORMOBJECTVIOLATION_H
