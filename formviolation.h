#ifndef FORMVIOLATION_H
#define FORMVIOLATION_H

#include <QWidget>
//#include "formborder.h"
#include "mapimagescene.h"
#include "formmark.h"
#include "formobjectviolation.h"

namespace Ui {
class FormViolation;
}

class FormViolation : public QWidget
{
    Q_OBJECT

public:
    explicit FormViolation(MapImageScene *scene_,TModeScene mode_,QWidget *parent = nullptr);
    ~FormViolation();

    int addNewBorder(TModeScene mode_);
    int addNewMark(QTreeWidget *treeWidget);
    virtual bool isModified() {return true;}
    virtual QPointF centerMap();
    TModeScene mode;

public slots:
    void setContinueMode();
    void setFinishMode();
    void setSquare(double);
    void deleteBorderForm(int data);
    void deleteMarkViolation();
    void setPointMark(QPoint p);

    void on_btnCancel_clicked();

protected:
    Ui::FormViolation *ui;
    FormBorder_ *getLastBorder();
    FormMark *formMark;
    MapImageScene *scene;
    void clear(bool refresh=false);
    int id;
    int idCurBorder;

    void getIdBorderFromDb();
//    int getIdBorderFromDb();
    QList<int> listDeleted;


signals:
    void insertViolationDB();
    void closeViolation(bool refresh);

//protected slots:

private slots:
    virtual void on_btnSave_clicked();
};


class FormNewViolation : public FormViolation
{
    Q_OBJECT

public:
     FormNewViolation(MapImageScene *scene_,TModeScene mode_,QWidget *parent = nullptr);
private:
     int insertDB();


private slots:
    void on_btnSave_clicked();
};

class FormDBViolation : public FormViolation
{
    Q_OBJECT

public:
     FormDBViolation(MapImageScene *scene_,TModeScene mode_,int id_,int typeV,QWidget *parent = nullptr);
     bool isModified();
     QPointF centerMap();
private:
     void init();
     void addBorder(int idBorder);

     bool existMark;
     void addDBMark();
     int idMark=0;
private slots:
      void on_btnSave_clicked();
protected slots:

    //void on_btnCancel_clicked();

};


#endif // FORMVIOLATION_H
