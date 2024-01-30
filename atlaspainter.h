#ifndef ATLASPAINTER_H
#define ATLASPAINTER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItemGroup>
#include <QPointF>
#include <QString>
#include <QColor>
#include <QObject>

typedef enum {noMode, calculateArea, selectError, selectOrientir } TModeScene;

class AtlasPainter: public QObject
{
    Q_OBJECT
public:
    AtlasPainter(QGraphicsScene *scene);
    ~AtlasPainter();

    void drawCircle(QRectF rect, QColor colorCrcl);
    void addPoint1(QRectF rect, QColor color);
    void addPoint(QPoint point);
    void addText(QString txt, QPointF p, QColor colorTxt, QFont txtFont);
    QVector<QPoint> getAreaPoints();
    void setMode(TModeScene mode_);
    void deleteBorder(int id,QVector <QPoint>&points);
    void deleteLastPoint();
    void drawLastLine();
    QVector<QPoint> getLastAreaPoints();
    double calculateAreaOnMap(QVector<QPoint> points);
    void setSizePix(QSizeF size_pix_);
    float meterWidthOfLine(QPoint startP, QPoint endP);
signals:
    void setFirstPoint();
    void getSquare(double);

private:
    QGraphicsScene *sc;
    QVector<QPoint> areaPoints;
    QVector<QPoint> lastAreaPoints;
    QSizeF size_pix;
    TModeScene mode;

    double widthLine;

    QList<QGraphicsLineItem*> listSelectedLines;
    void drawLine(QPoint p1,QPoint p2,QString colorLineSq);
    QString colorLineSq();
};  

#endif // ATLASPAINTER_H


