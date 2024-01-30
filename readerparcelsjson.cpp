#include "readerparcelsjson.h"
#include <iostream>
#include <QFile>
#include "settings.h"
#include <QProgressDialog>
#include "handlemesserror.h"
#include "domparser.h"

readerParcelsJson::readerParcelsJson(QTreeWidget *tree)
{
     treeWidget = tree;
}



QSizeF readerParcelsJson::readFile()
{
    QString nameFile=QString("%1/parcels.json").arg(Settings::Instance().getDir());
    QFile file(nameFile);
    if(!file.exists())
    {
        QString lastError =QString("Не найден файл %1").arg(nameFile);
        throw  handleMessError(lastError,"Получение информации о кадастровой карте");
    }


    QJsonDocument _json;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QString lastError =QString("Ошибка чтения файла %1").arg(nameFile);
        throw  handleMessError(lastError,"Получение информации о кадастровой карте");
    }
    _json = QJsonDocument::fromJson(file.readAll());
    QJsonArray pixel_size_m = _json["pixel_size_m"].toArray();
    QJsonArray parcels = _json["parcels"].toArray();
    for(int i=0;i<parcels.count();i++)
    {
        QJsonObject obj1=parcels[i].toObject();
        objectXml object_tree;
        object_tree.cadastNum=obj1["cad_number"].toString();
        object_tree.name=obj1["cad_number"].toString();
        object_tree.cost=obj1["cost"].toString();
        object_tree.adrStr=(obj1["address"].toObject())["readable_address"].toString();
        object_tree.byDoc=obj1["permitted_use"].toString();
        //listOfPoints<<QVariant(QPointF(ordinate.toElement().attribute("X").toDouble(), ordinate.toElement().attribute("Y").toDouble()));

        QJsonArray points=obj1["points"].toArray();
        for(int j=0;j<points.count();j++)
        {
            QJsonArray p=points[j].toArray();
            object_tree.listOfPoints<<QVariant(QPointF(p[0].toInt(),p[1].toInt()));

        }
        object_tree.addToTree(treeWidget);
    }
    return QSizeF(pixel_size_m[0].toDouble(),pixel_size_m[1].toDouble());
}
