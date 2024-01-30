#include "listpolygons.h"
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
//#include <QListWidgetItem>
#include <QMessageBox>

listPolygons::listPolygons(QWidget *parent): QListWidget(parent)
{

}


void listPolygons::addPolygon(QVector<QPoint> &vector)
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(QString("%1 ").arg(this->count()+1));
    QList<QVariant> list;
    foreach(QPoint p, vector)
        list<<p;
    newItem->setData(Qt::UserRole, list);
    insertItem(this->count(), newItem);
}


void listPolygons::mousePressEvent(QMouseEvent *e)
{   
    Q_UNUSED(e);

}

void listPolygons::mouseReleaseEvent(QMouseEvent *e)
{  
    Q_UNUSED(e);

    if (item != nullptr)  {
      if (!item->text().isNull() && !item->text().isEmpty())  {
          QListWidgetItem *newItem = new QListWidgetItem;
          newItem->setData(Qt::UserRole, item->data(Qt::UserRole));
          newItem->setText(item->text());

          this->addItem(newItem);
      }
    }

}




