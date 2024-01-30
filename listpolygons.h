#ifndef LISTPOLYGONS_H
#define LISTPOLYGONS_H
#include <QListWidget>
#include <QListWidgetItem>


class listPolygons : public QListWidget
{
     Q_OBJECT
public:
    listPolygons(QWidget *parent = nullptr);


public slots:
    void addPolygon(QVector<QPoint> &vector);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;


private:
    QListWidgetItem *item;


};

#endif // LISTPOLYGONS_H
