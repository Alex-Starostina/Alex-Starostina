#ifndef PARSERFILETAB_H
#define PARSERFILETAB_H

#include <QList>
#include <QPointF>

class parserFileTab
{
public:
    parserFileTab(const QString &vFileName);
    QList<QPointF> getCoordRaster();
private:
    QString fileName;
    QString context;
    QPointF getCoord(QString str);

};

#endif // PARSERFILETAB_H
