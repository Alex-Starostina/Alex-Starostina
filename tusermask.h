#ifndef TUSERMASK_H
#define TUSERMASK_H
#include <QImage>
#include <QPixmap>
#include <QObject>

class TUserMask : public QObject
{
     Q_OBJECT
public:
    TUserMask();
    void addPixMap(QPixmap source);
    void setStartPixMap(QPixmap source);

    QPoint getPointForView();
    QPoint getPointForViewPrev();
    void clearPolygon(QPolygon &polygonNew);
    void revertPolygon(QPolygon &polygonNew);
    void createListPolygons();
private:
    QPoint pos;
    QPoint pos0;
    //QImage img;
    QPixmap pixmap;
    QPixmap pixmap_start;
    QList<QPoint> listPos;
    bool isStart=true;

    QList<QPolygon> listPolygon;
    bool containsPoint(const QPoint &point);
};

#endif // TUSERMASK_H
