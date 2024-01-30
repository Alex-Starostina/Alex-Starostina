#ifndef READERPARCELSJSON_H
#define READERPARCELSJSON_H

class QString;
class QTreeWidget;
class QTreeWidgetItem;
class QSizeF;

class readerParcelsJson
{
public:
    readerParcelsJson(QTreeWidget *tree);

    QSizeF readFile();
private:
    QTreeWidget *treeWidget;
};

#endif // READERPARCELSJSON_H
