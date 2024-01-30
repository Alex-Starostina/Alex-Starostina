#include "maskitem.h"
#include <QFile>
#include "handlemesserror.h"
#include <QBitmap>
#include "settings.h"

maskItem::maskItem(TSceneMask type_,QGraphicsScene *scene_):QGraphicsPixmapItem(),type(type_),parentScene(scene_)
{

}

void  maskItem::setFileName()
{
    switch (type)
    {
    case Buildings:
       fileName=Settings::Instance().getFileBuildings();
       color=Qt::red;
       break;
    case Borders:
       fileName=Settings::Instance().getFileBorders();
       color=Qt::red;
       break;
    case CMap:
       fileName=Settings::Instance().getFileCMap();
       color=Qt::blue;
       break;
    }
}

QString maskItem::getFileName()
{
    return fileName;
}

void maskItem::load()
{
    setFileName();
    if (QFile::exists(fileName))
    {
        QImage img;
        if (!img.load(fileName))
        {
            QString strMess=QString("Не удалось загрузить файл %1").arg(fileName);
            throw handleMessError(strMess,"Загрузка данных");
        }

        img.invertPixels();
        QBitmap mask = QBitmap::fromImage(img);

//if (!mask.isNull()) {
        QPixmap pixmap(mask.size());
        pixmap.fill(color);
        pixmap.setMask(mask);
        setPixmap(pixmap);
//}
        if(parentScene!=nullptr)
        {

            if(this->scene()!=nullptr) parentScene->removeItem(this);
            parentScene->addItem(this);
        }
        isLoad=true;
    }
    else
    {
        QString strMess=QString("Файл не найден %1 ").arg(fileName);
        throw handleMessError(strMess,"Загрузка данных");
    }

}
