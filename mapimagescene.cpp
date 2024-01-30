#include <QDebug>
#include <QScrollBar>
#include <QBitmap>
#include <QVector>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include "settings.h"

#include "mapimagescene.h"
#include "handlemesserror.h"


MapImageScene::MapImageScene()
{
    isStandardCursor = true;

    mode=noMode;
    widthLine = Settings::Instance().getWidthLine();

//    addItem(&mapImage);
//    isCross=false;
    maskOfBuildings=new maskItem(Buildings,this);
    maskOfBorders=new maskItem(Borders,this);
    maskCMap=new maskItem(CMap,this);
    maskOfBuildingsImg=new TUserMask();
    painter = new AtlasPainter(this);

    connect(painter,SIGNAL(getSquare(double)),this,SIGNAL(getSquare(double)));
    connect(painter,SIGNAL(setFirstPoint()),this,SIGNAL(setFirstPoint()));
}

MapImageScene::~MapImageScene()
{

}


void MapImageScene::clearMarkupOnPhoto()
{
    QList<QGraphicsItem*> items = this->items();
    for (QGraphicsItem *item : items) {
        if (item != nullptr) {
          if (item->type() == 5) { //QGraphicsPolygonItem
            auto polyg = static_cast<QGraphicsPolygonItem*>(item);
            if (polyg->brush().color() == QColor(255,255,255,160)) removeItem(polyg);
            if (polyg->brush().color() == QColor(255,255, 0, 70)) removeItem(polyg);
            if (polyg->brush().color() == Qt::yellow)
                removeItem(polyg);
          }

          if (item->type() == 4) { //QGraphicsEllipseItem
//            auto ell1 = static_cast<QGraphicsEllipseItem*>(item);
//            if (ell1->brush().color() != QColor("red")) {
              removeItem(item);
//            }
          }
          if (item->type() == 8) //QGraphicsTextItem
              removeItem(item);
          if ( item->type() == 6) {
            QGraphicsLineItem *line = static_cast<QGraphicsLineItem*>( item);
                if (line != 0) {
                    //удаляем штрих-пунктирную линию площади
                    if (line->pen().style() == Qt::DashLine)  removeItem(line);
                }
          }
        }
    }

    areaPoints().clear();
}

void MapImageScene::loadMap(QString fileName)
{
    if (QFile::exists(fileName))
    {
        for (auto *item: items())
            removeItem(item);
        qApp->processEvents();
        img = QImage(fileName);
        addItem(&mapImage);
        setSceneRect(0, 0, img.width(), img.height());

        QPixmap pxmp;
        if (pxmp.load( fileName, "jpeg"))
        {
            mapImage.setPixmap(pxmp);
        }

        else {
            throw handleMessError(QString("Ошибка загрузки файла %1").arg(fileName), "Загрузка данных");
        }
        qApp->processEvents();
        loadMasks();
    }
    else {
        throw handleMessError(QString("Файл %1 не найден.").arg(fileName), "Загрузка данных");
    }
}

void MapImageScene::loadMasks()
{
    maskOfBuildings->load();
    qApp->processEvents();
    maskOfBuildingsImg->setStartPixMap(maskOfBuildings->pixmap());
    maskOfBorders->load();
    qApp->processEvents();
    maskOfBuildingsImg->addPixMap(maskOfBorders->pixmap());
    maskCMap->load();
    qApp->processEvents();
}

void MapImageScene::loadMap(QString fileName, QSizeF size_pix_)
{
    if (QFile::exists(fileName))
    {

        /*rectMap=coord;
        if(!isValidCoord())
        {
            QString strMess="Не заданы координаты";
            throw handleMessError(strMess,"Загрузка данных");
        }*/
        for (QGraphicsItem *item : items()) removeItem(item);
        addItem(&mapImage);
        setSceneRect(0, 0, QImage(fileName).width(), QImage(fileName).height());

        QPixmap pixmap;
        if (pixmap.load(fileName))
        {
            mapImage.setPixmap(pixmap);
            img=mapImage.pixmap().toImage();
            mask=QBitmap(pixmap);
            //deltaX=abs(pixmap.width()/(rectMap[2].x()-rectMap[1].x()));
            //deltaY=abs(pixmap.height()/(rectMap[1].y()-rectMap[0].y()));
            painter->setSizePix(size_pix_);
        }
        else
        {

            QString strMess=QString("Ошибка загрузки %1 ").arg(fileName);
            throw handleMessError(strMess,"Загрузка данных");

        }
        loadMasks();
//        loadMasks1();
    }
    else
    {

        QString strMess=QString("Файл не найден %1 ").arg(fileName);
        throw handleMessError(strMess,"Загрузка данных");
    }
}


void MapImageScene::updateListPolygon()
{
    maskOfBuildingsImg->createListPolygons();
}

void MapImageScene::loadMasks1()
{
    maskOfBuildingsImg->setStartPixMap(maskOfBuildings->pixmap());
    maskOfBuildingsImg->addPixMap(maskOfBorders->pixmap());
}

void MapImageScene::setVisibleBorder(bool isVisible)
{
    maskOfBorders->setVisible(isVisible);
}

void MapImageScene::setVisibleBuildings(bool isVisible)
{
    maskOfBuildings->setVisible(isVisible);
}

void MapImageScene::setVisibleCmap(bool isVisible)
{
    maskCMap->setVisible(isVisible);
}


 /*bool MapImageScene::isValidCoord()
 {
     return (rectMap.count()==4);
 }*/


/*uint MapImageScene::calculateAreaOnMap(QVector<QPoint> pointsArea)
{
     int w = 1000; // ширина участка местности в метрах
     int h = 1000; // длина участка местности в м.
     if (Settings::Instance().getWidthArea() > 0) w = Settings::Instance().getWidthArea();
     if (Settings::Instance().getHeightArea() > 0) h = Settings::Instance().getHeightArea();

     if (pointsArea.count() < 3) {
         QMessageBox msg;
         msg.setIcon(QMessageBox::Warning);
         msg.setText("Указано менее 3x точек участка расчета площади!");
         msg.setWindowTitle( Settings::Instance().getAppName());
         msg.exec();
         return 0;
     }

     QPolygon polyg(pointsArea);
     QRect boundRect = polyg.boundingRect();
     QPoint topLeftP = boundRect.topLeft();
     int goriz = topLeftP.rx();
     int maxG = boundRect.topRight().rx()+1;
             //goriz + boundRect.width()+1;
     int vert = topLeftP.ry();
     int maxV = boundRect.bottomRight().ry()+1;
             //vert + boundRect.height()+1;
     QPoint p;
     uint sq = boundRect.width() * boundRect.height();

     while (goriz < maxG) {
         for (int i=vert; i<maxV; i++) {
           p.setX(goriz);
           p.setY(i);
           if (!polyg.containsPoint(p, Qt::WindingFill))
               sq--;
         }

       goriz++;
     }

     //расчет площади в кв. метрах
     auto allS = (this->width() * this->height()) / (w * h);
     sq = sq / allS;

     return sq;

}*/






void MapImageScene::setMode(TModeScene mode_)
{
    mode=mode_;
    painter->setMode(mode);
}


void MapImageScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (!isStandardCursor)  views().at(0)->viewport()->setCursor(Qt::PointingHandCursor);
//qDebug() << "mode " << mode;

    if (mouseEvent->button() == Qt::RightButton) {
//<<<<<<< HEAD
//        if (mode == calculateArea) {
//            //pp = mouseEvent->scenePos();
//            listPPs.push_back( mouseEvent->scenePos());
//        }

//        lastPoint = mouseEvent->scenePos();
//        areaPoints.push_back(lastPoint.toPoint());
//=======
      lastPoint = mouseEvent->scenePos();
      //areaPoints.push_back(lastPoint.toPoint());
//>>>>>>> a55584508a4b8c07121bebf5a1729ecbfaed666b
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void MapImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if (!isStandardCursor)  views().at(0)->viewport()->setCursor(Qt::PointingHandCursor);

  if (mouseEvent->button() == Qt::RightButton)
  {
    if (mode==calculateArea || mode==selectError )
    {
         painter->addPoint(lastPoint.toPoint());
    }
    if (mode == selectOrientir)
    {
        emit drawMark(lastPoint.toPoint());
        setMode(noMode);
    }
  }
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


QVector<QPoint> MapImageScene::areaPoints()
{
   return painter->getLastAreaPoints();
}


void MapImageScene::deleteLastPoint()
{
    painter->deleteLastPoint();
}

void MapImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)
{
    Q_UNUSED(evt)
    if (!isStandardCursor)  views().at(0)->viewport()->setCursor(Qt::PointingHandCursor);
}

void MapImageScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    Q_UNUSED(mouseEvent)   
    if (!isStandardCursor) {
      isStandardCursor = true;
    }

//    if (mode == selectOrientir) mode = noMode;

//    if(!((mode==calculateArea) || (mode==selectError))) return;
    emit finishDrawBorder_doubleClick();
    finishDrawBorder();
}

void MapImageScene::finishDrawBorder()
{
    //painter->deleteLastBorder();
    mode=noMode;
    painter->setMode(mode);
}

void MapImageScene::clearPolygon(QPolygon &polygon)
{
    Q_UNUSED(polygon)
    //qDebug()<<"MapImageScene::clearPolygon";
    //maskOfBuildingsImg->clearPolygon(polygon);
}


void MapImageScene::revertPolygon(QPolygon &polygon)
{
    Q_UNUSED(polygon)
    //maskOfBuildingsImg->revertPolygon(polygon);
}


QVector<QPoint>  MapImageScene::getPointsOfBorder()
{
    return painter->getAreaPoints();
}

/*void MapImageScene::deleteLastBorder()
{
   painter->deleteLastBorder();
}*/

void MapImageScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{   
    views().at(0)->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    views().at(0)->setResizeAnchor(QGraphicsView::AnchorUnderMouse); //


//   views().at(0)->verticalScrollBar()->blockSignals(false);
//   views().at(0)->horizontalScrollBar()->blockSignals(false);

    if(event->delta() > 0)
       views().at(0)->scale(1.25, 1.25);
    else
       views().at(0)->scale(0.8, 0.8);
}


/*bool MapImageScene::contain(QList<QVariant> listOfPoints)
{
  if (!rectMap.isEmpty()) {
    for(int i=0;i<listOfPoints.count();i++)
    {
        QPointF p=listOfPoints[i].toPointF();       
        if(p.x()>=rectMap[3].x()&&p.x()<=rectMap[1].x()
                &&p.y()>=rectMap[3].y() &&p.y()<=rectMap[1].y())
            return true;
    }

    return false;
  }
  else
      return true;
}*/

QPointF MapImageScene::convertPoint(QPointF p,int width,int height)
{
    qreal kx=this->width()/width;
    qreal x=p.x()*kx;
    qreal ky=this->height()/height;
    qreal y=p.y()*ky;;
    return QPointF(x,y);
}

QGraphicsPolygonItem* MapImageScene::drawGeoPolygon(QList<QVariant> listOfPoints, QColor color)
{
    QPolygonF polygon;
    for(int i=0;i<listOfPoints.count();i++)
    {
       polygon<< listOfPoints[i].toPointF();
    }
    return drawPolygon(polygon, color);
}


QGraphicsRectItem* MapImageScene::drawErrorRect(QList<QVariant> listOfPoints,QColor color)
{
    QList<QGraphicsItem*> listItems = items();
    foreach(QGraphicsItem *item,listItems)
    {

        if(item->data(100)==100)
        removeItem(item);

    }

    QPoint p1(listOfPoints[0].toInt(),listOfPoints[1].toInt());
    QPoint p2(listOfPoints[2].toInt(),listOfPoints[3].toInt());
    QRect rect(p1,p2);
    QPen pen(color);
    pen.setWidth(5);
    pen.setColor(color);
    QGraphicsRectItem* item=addRect(rect,pen);
    item->setData(100,100);
    return item;
}

/*QPolygonF MapImageScene::getPolygonFromGeoCoord(QList<QVariant> listOfPoints)
{
    QPolygonF polygon;
    if(!contain(listOfPoints)) return polygon;
    for(int i=0;i<listOfPoints.count();i++)
    {
       polygon<< toPixel(listOfPoints[i].toPointF());
    }
    return polygon;
}*/

QGraphicsPolygonItem*  MapImageScene::drawPolygon(const QPolygonF &polygon,QColor color)
{
   //controlPolygon *curPolygon=new controlPolygon(color,polygon);
   //this->addItem(curPolygon);
   //return curPolygon;
   QPen pen(color);
   //qDebug() << "width " << Settings::Instance().getWidthLine();
   pen.setWidth(Settings::Instance().getWidthLine());

   //   QPolygonF tesPol;
   //   tesPol << QPointF(0,0) <<QPointF(100,0) << QPointF(100, 100) <<QPointF(0,100);
   //   addPolygon(tesPol, pen/*, QBrush(color)*/);

   return addPolygon(polygon, pen);//,QBrush(color)
}



void MapImageScene::drawEllipse(QPointF p)
{
    if((p.x()>img.width())&&(p.y()>img.height())) return;
    QColor color = Qt::red;
    int radius = Settings::Instance().getWidthLine();
    int d=radius/2;
    addEllipse(p.x()-d, p.y()-d,radius,radius,QPen(color), QBrush(color));
}

/*QPointF MapImageScene::toPixel(QPointF point)
{
    QPointF newPoint;

    newPoint.setX((point.y()-rectMap[0].y())*deltaY);
    newPoint.setY((rectMap[0].x()-point.x())*deltaX);
     return newPoint;
}*/



void MapImageScene::addTextOnMap(QString txtLabel, QFont fontLabel,  QRect rect, QPoint leftTop)
{
    QColor txtColor("black");

    this->addPolygon(QPolygonF(QRectF(rect)), QPen(QColor(0, 0, 154, 230)), QBrush(QColor(255,255,255,160)));

    QGraphicsTextItem *txtItem = this->addText(txtLabel, fontLabel);
    txtItem->setPos(static_cast<QPointF>(leftTop));
    //txtItem->setTextWidth(txtWidth);
    //txtItem->setHtml(outText);
    txtItem->setDefaultTextColor(txtColor);
}

QPoint MapImageScene::searchNext()
{    
    QPoint centerP=maskOfBuildingsImg->getPointForView();
    if(centerP.x()!=-1)
      drawEllipse(centerP);
    return centerP;
}


QPoint MapImageScene::searchPrev()
{
    QPoint centerP=maskOfBuildingsImg->getPointForViewPrev();
    return centerP;
}


void MapImageScene::saveScene(QString fileName)
{
    QImage *image=new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    image->fill(Qt::transparent);
    QPainter painter(image);
    render(&painter);
    image->save(fileName);
    //return image;
}


void MapImageScene::saveToPng(QRect rect,QString fileName)
{

    /*QImage img1(int(item->boundingRect().width())+1,int(item->boundingRect().height())+1, QImage::Format_ARGB32);

    int x1=item->boundingRect().bottomRight().x();
    int y1=item->boundingRect().bottomRight().y();
    int x0=item->boundingRect().topLeft().x();
    int y0=item->boundingRect().topLeft().y();*/
    QImage img1(int(rect.width())+1,int(rect.height())+1, QImage::Format_ARGB32);

        int x1=rect.bottomRight().x();
        int y1=rect.bottomRight().y();
        int x0=rect.topLeft().x();
        int y0=rect.topLeft().y();
    //qDebug()<<x1<<y1<<x0<<y0;
    int i1,j1;
    i1=0;
    for(int i=x0;i<x1;i++)
    {
       j1=0;
       for(int j=y0;j<y1;j++)
       {
           if(img.valid(i,j)&&img1.valid(i1,i1))
           img1.setPixel(i1,j1,img.pixel(i,j));
           j1++;
       }
       i1++;
    }
   img1.save(fileName);
}
