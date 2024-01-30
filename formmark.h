#ifndef FORMMARK_H
#define FORMMARK_H

#include <QWidget>
#include "mapimagescene.h"

namespace Ui {
class FormMark;
}


struct findedAreaEl {
    public:

          QString adr = "";  //адрес объекта недвиж-ти
          QString kadNum = ""; //кадастровый № ОН
          QString price = ""; //cтоимость объекта недвиж-ти
          uint ind = -1; //номер в списке объектов
          QList<QVariant> coords;
};

class FormMark : public QWidget
{
    Q_OBJECT

public:
    FormMark(MapImageScene *scene_,modeObjectViolation mode_,QWidget *parent=nullptr);
    ~FormMark();
    int getId();   
    void setMode(modeObjectViolation mode_);
    virtual void save(int idViolation);
    virtual void setPoint( QPoint point_);
    virtual void cancel(){}
    virtual bool isNew(){return false;}
    void deleteFromDB();
    void deleteFromScene();
    QPoint point;
protected:
    Ui::FormMark *ui;
    int id;

    void draw();

    MapImageScene *scene;
    modeObjectViolation mode;

    findedAreaEl findedAreaEl1;

signals:
    void deleteMark();
private slots:
    void on_btnDelete_clicked();
    void on_btnCancelDraw_clicked();
};

class FormNewMark : public FormMark
{
    Q_OBJECT

public:
    FormNewMark(MapImageScene *scene_,QTreeWidget* treeWidget,QWidget *parent=nullptr);
    virtual void setPoint(QPoint point_);
    void cancel();
    void save(int idViolation);
    bool isNew(){return true;}
private:
    QTreeWidget* treeWidget;
    void insertDB();
    bool findAreaInfoByPoints();
};

class FormDBMark : public FormMark
{
    Q_OBJECT

public:
    FormDBMark(MapImageScene *scene_,int id_,QWidget *parent=nullptr);
private:
    void initFromDB();
};

#endif // FORMMARK_H
