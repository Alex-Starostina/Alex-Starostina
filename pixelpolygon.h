#ifndef PIXELPOLYGON_H
#define PIXELPOLYGON_H
#include <QObject>
#include <QImage>

const QRgb colorWhite=4294967295;


typedef enum {North, East, South, West} TDirectional;
class PixelPolygon:public QObject
{
      Q_OBJECT

public:
       PixelPolygon(QImage *mask_);
       void getPolygon();
       QPolygonF findNewPolygon();
       QPolygonF AlgorithmBeatle();

private:
       QImage *mask;
       QPoint findNextUpPoint(QLine line);
       QPoint findNextDownPoint(QLine line);
       QLine findStartLine();
       QPolygonF createPolygon(const QPolygonF &polygonUp,const QPolygonF &polygonDown);





};

#endif // PIXELPOLYGON_H
