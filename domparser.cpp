//#include <QtGui>
#include <QtXml>
#include <iostream>
//#include <QDebug>
#include "domparser.h"
#include <QTreeWidget>



//-------------------------------------objectXml---------------------------------
void objectXml::addToTree(QTreeWidget *tree)
{
     QTreeWidgetItem *treeItem = new QTreeWidgetItem(tree->invisibleRootItem(),type);
     //QString str = QString("%1 %2").arg(name).arg(num);
    // treeItem->setText(0, str);
     treeItem->setText(0, cadastNum);
     treeItem->setText(1, adrStr);
     treeItem->setText(2, byDoc);
     treeItem->setText(3, cost);
     treeItem->setData(0, Qt::UserRole, QVariant(listOfPoints));
     treeItem->setData(0,Qt::ToolTipRole,QString("%1 %2").arg(cadastNum).arg(adrStr));
     //treeItem->setData(0,Qt::DecorationRole,QColor(Qt::red));
}

//---------------------------------DomParser-----------------------
DomParser::DomParser(QTreeWidget *tree,MapImageScene *scenePng_)
{
    treeWidget = tree;
    scenePng=scenePng_;
    numPlot=1;

}

bool DomParser::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QString err = QString("Ошибка чтения файла %1: %2").arg(fileName).arg(file.errorString());
       return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine,&errorColumn))
    {
        err = QString("Error: Parse error at line %1, column %2: %3").arg(errorLine).arg(errorColumn).arg(errorStr);
        return false;
        }
    QDomElement root = doc.documentElement();
    QDomElement cadastralBlocks = root.firstChildElement("CadastralBlocks");
    if (cadastralBlocks.isNull())
    {
        return false;
    }
    QDomNode cadastralBlock = cadastralBlocks.firstChild();
    if (cadastralBlock.isNull())
    {
        return false;
    }
    QDomElement parcels = cadastralBlock.firstChildElement("Parcels");
    parseParcelsElement(parcels);
   QDomElement realEls = cadastralBlock.firstChildElement("ObjectsRealty");
    parseObjectsRealtyElement(realEls);
    return true;
}

void DomParser::parseParcelsElement(const QDomElement &parcels)
{
    if ( parcels.isNull()) return;

    QDomNodeList parcelList = parcels.childNodes();
    for (int i=0; i<parcelList.size(); i++)
    {
        QDomNode parcel=parcelList.at(i);
        parseParcelElement(parcel);
    }
}




void DomParser::parseParcelElement(QDomNode &parcel)
{
    objectXml object;
    object.name="Участок";
    object.type=1000;
    object.cadastNum = parcel.toElement().attribute("CadastralNumber");
    object.adrStr=parseLocationElement(parcel);
    object.byDoc=parseUtilizationElement(parcel);
    object.cost=parseCadastralCostElement(parcel);
    object.listOfPoints=parseEntitySpatialElement(parcel);
     /*if (scenePng->contain(object.listOfPoints))
    {
        object.num=numPlot++;
        object.addToTree(treeWidget);
        //scenePng.getPlot(listOfPoints,false);
        //рисование участков на tif изображении
        //sceneTif.drawPlot();
    }*/
}

QString DomParser::parseLocationElement(QDomNode &parcel)
{

    QDomElement location = parcel.firstChildElement("Location");
    return  parseAddressElement(location);

}

QString DomParser::parseAddressElement(QDomNode &node)
{

    QString adrStr;
    if (node.isNull()) return adrStr;
    QDomElement adr = node.firstChildElement("Address");
    if (adr.firstChildElement("ns2:Note").isNull()) return adrStr;
    return  adr.firstChildElement("ns2:Note").text();
}


QString DomParser::parseUtilizationElement(QDomNode &parcel)
{

    QDomNode utiliz = parcel.firstChildElement("Utilization");
    if (utiliz.isNull()) return QString();
    return utiliz.toElement().attribute("ByDoc");
 }

QString DomParser::parseCadastralCostElement(QDomNode &parcel)
{
    QDomElement cadastralCost = parcel.firstChildElement("CadastralCost");
    if (cadastralCost.isNull()) return QString();
    return cadastralCost.attribute("Value");
}


QList<QVariant> DomParser::parseEntitySpatialElement(QDomNode &parcel)
{
    QList<QVariant> listOfPoints;
    QDomElement entitySpatial = parcel.firstChildElement("EntitySpatial");
    if ( entitySpatial.isNull()) return listOfPoints;

    QDomNode spatialElement = entitySpatial.namedItem("ns3:SpatialElement");
    if (spatialElement.isNull())  return listOfPoints;
    if (spatialElement.hasChildNodes())
    {
        QDomNodeList ns3SpelUnitList = spatialElement.childNodes();

        for (int j=0; j<ns3SpelUnitList.size(); j++)
        {
            QDomNode coordEl = ns3SpelUnitList.at(j);
            if (!coordEl.isNull())
            {
                QDomNode ordinate = coordEl.firstChildElement("ns3:Ordinate");
                if (!ordinate.isNull())
                listOfPoints<<QVariant(QPointF(ordinate.toElement().attribute("X").toDouble(), ordinate.toElement().attribute("Y").toDouble()));

            }
        }
    }
    return  listOfPoints;
}

 void DomParser::parseObjectsRealtyElement( QDomNode &realEls)
 {

         QDomNodeList objRealtyList = realEls.toElement().elementsByTagName("ObjectRealty");

         for (int i=0; i<objRealtyList.size(); i++)
         {
             QDomNode node = objRealtyList.at(i);
             parseObjectRealtyElement(node);
         }
 }

void DomParser::parseObjectRealtyElement( QDomNode &node)
 {
     QDomNode buildingTag = node.namedItem("Building");
     if (buildingTag.isNull()) return;
     objectXml object;
     object.name="Здание";
     object.type=1001;
     object.byDoc = "";
     if (!buildingTag.toElement().attribute("CadastralNumber").isNull())
     {
       object.cadastNum =buildingTag.toElement().attribute("CadastralNumber");
     }

     object.cadastNum = buildingTag.toElement().attribute("CadastralNumber");
     object.adrStr= parseAddressElement(buildingTag);

     object.cost=parseCadastralCostElement(buildingTag);
     object.listOfPoints=parseEntitySpatialElement(buildingTag);

     /*if (scenePng->contain(object.listOfPoints))
     {
         object.num=numPlot++;
         object.addToTree(treeWidget);
         //scenePng.getPlot(listOfPoints,false);
         //рисование участков на tif изображении
         //sceneTif.drawPlot();
     }*/
 }


void DomParser::parseEntryElement(const QDomElement &element,
                                  QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, element.attribute("term"));

    QDomNode child = element.firstChild();
    while (!child.isNull()) {
        if (child.toElement().tagName() == "entry") {
            parseEntryElement(child.toElement(), item);
        } else if (child.toElement().tagName() == "page") {
            parsePageElement(child.toElement(), item);
        }
        child = child.nextSibling();
    }
}

void DomParser::parsePageElement(const QDomElement &element,
                                 QTreeWidgetItem *parent)
{
    QString page = element.text();
    QString allPages = parent->text(1);
    if (!allPages.isEmpty())
        allPages += ", ";
    allPages += page;
    parent->setText(1, allPages);
}
