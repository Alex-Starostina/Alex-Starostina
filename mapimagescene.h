#ifndef MAPIMAGESCENE_H
#define MAPIMAGESCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneEvent>
#include <QBitmap>
#include <QTreeWidgetItem>
#include <QVector>
#include "pixelpolygon.h"
#include <QMenu>
#include <QRectF>
#include <QMap>
#include "maskitem.h"
#include <QImage>
#include "tusermask.h"
#include "controlpolygon.h"
#include "atlaspainter.h"

enum modeObjectViolation{startDraw_, continueDraw_,finishDraw_,None};



class MapImageScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MapImageScene();
    ~MapImageScene();
    QImage img;
    QBitmap mask;



    AtlasPainter *painter;
    bool isStandardCursor;

    //QVector<QPoint> areaPoints; //коорд-ы отмеч. точек
//    QMap<int, QVector<QPoint>> areaMap; //cписок векторов areaPoints

//    QVector<QPointF> listPPs; //вектор координат точек для расчета длин сторон
//    bool meterWidthes;
    void clearMarkupOnPhoto(); //


    void loadMap(QString fileName);
    void loadMap(QString fileName,QSizeF size_pix_);
//    QList<QPointF> getCoord();
   // bool isValidCoord();


//    bool lastIsCross() {return isCross;}
   // bool contain(QList<QVariant> listOfPoints);

    void drawEllipse(QPointF p);
    //QPointF toPixel(QPointF point);


  // QGraphicsPolygonItem*  drawPolygon(QList<QVariant> listOfPoints,QColor color=Qt::blue);
    QGraphicsPolygonItem*  drawPolygon(const QPolygonF &polygon,QColor color=Qt::blue);
    QGraphicsPolygonItem*  drawGeoPolygon(QList<QVariant> listOfPoints,QColor color=Qt::blue);
    QGraphicsRectItem* drawErrorRect(QList<QVariant> listOfPoints,QColor color=Qt::red);

    //QGraphicsPolygonItem*  convertAndDrawPolygon(QList<QVariant> listOfPoints,int width,int height,QColor color=Qt::blue);
    //QGraphicsPolygonItem*  convertAndDrawPolygon(const QPolygonF &polygon,int width,int height,QColor color=Qt::blue);
    //QPolygonF getPolygonFromGeoCoord(QList<QVariant> listOfPoints);

    void addTextOnMap( QString txtLabel,  QFont fontLabel, QRect rect, QPoint leftTop);
    //uint calculateAreaOnMap(QVector<QPoint>);




    QPoint searchNext();
    QPoint searchPrev();

    void setMode(TModeScene mode_);
    void setVisibleBorder(bool isVisible);
    void setVisibleBuildings(bool isVisible);
    void setVisibleCmap(bool isVisible);

    QVector<QPoint>  getPointsOfBorder();
    //void deleteLastBorder();
    void setIdBorder(int id);
    void deleteLastPoint();
    //void drawBorder(int idBorder,QVector <QPoint>&points);
    void clearPolygon(QPolygon &polygon);
    void revertPolygon(QPolygon &polygon);
    void loadMasks();
    void loadMasks1();
    void updateListPolygon();
    void saveScene(QString fileName);
    TModeScene getMode(){return mode;}
    void saveToPng(QRect rect,QString fileName);
    QVector<QPoint> areaPoints();
public slots:
    void finishDrawBorder();

private:
    int idBorder=0;
    QGraphicsPixmapItem mapImage;
    maskItem *maskOfBuildings;
    maskItem *maskOfBorders;
    maskItem *maskCMap;
    TUserMask *maskOfBuildingsImg;

    qreal widthLine;
    QPointF lastPoint;


    TModeScene mode;


    //QList<QPointF> rectMap;

    bool isCross;    
    QPointF convertPoint(QPointF p, int width, int height);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* ) override;



   // void contextMenuEvent(QContextMenuEvent *event);
signals:
   void selectErrorPolygon(QVector<QPoint> &list);
   void selectAreaPolygon(QVector<QPoint> &list);
   void setFirstPoint();
   void getSquare(double);
   void finishDrawBorder_doubleClick();
   void drawMark(const QPoint &p);
};




#endif // MAPIMAGESCENE_H
