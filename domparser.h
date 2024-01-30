#ifndef DOMPARSER_H
#define DOMPARSER_H
#include <QVariant>
#include "mapimagescene.h"

class QDomElement;
class QString;
class QTreeWidget;
class QDomNode;
class QTreeWidgetItem;


class objectXml
{
public:
    int num;
    QString name; //тип объекта (зем. участок, здание) + номер
    int type;//1000-Участок,1001-здание
    QString cadastNum;
    QString adrStr;
    QString byDoc;
    QString cost;
    QList<QVariant> listOfPoints;
    void addToTree(QTreeWidget *tree);

};

class DomParser
{
public:
    DomParser(QTreeWidget *tree,MapImageScene *scenePng_);

    bool readFile(const QString &fileName);

private:
    void parseParcelsElement(const QDomElement &element);
    void parseParcelElement( QDomNode &parcel);
    void parseObjectsRealtyElement(QDomNode &realEls);
    QString parseLocationElement(QDomNode &parcel);
    QString parseUtilizationElement(QDomNode &parcel);
    QString  parseCadastralCostElement(QDomNode &parcel);
    QList<QVariant> parseEntitySpatialElement(QDomNode &parcel);
    QString parseAddressElement(QDomNode &node);
    void parseObjectRealtyElement( QDomNode &node);
    void parseEntryElement(const QDomElement &element,QTreeWidgetItem *parent);
    void parsePageElement(const QDomElement &element,QTreeWidgetItem *parent);
    QTreeWidget *treeWidget;
    MapImageScene *scenePng;
    QString err;
    int numPlot;
};

#endif
