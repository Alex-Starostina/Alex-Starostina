#ifndef MASKITEM_H
#define MASKITEM_H
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
typedef enum {Buildings,Borders,CMap} TSceneMask;

class maskItem : public QGraphicsPixmapItem
{
public:
    maskItem(TSceneMask type,QGraphicsScene *scene_);
    void load();
    QString getFileName();
    void setFileName();
private:
    bool isLoad=false;
    QString fileName;
    QColor color;
    TSceneMask type;
    QGraphicsScene *parentScene;
};

#endif // MASKITEM_H
