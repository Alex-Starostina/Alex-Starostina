#include "parserfiletab.h"
#include <QFileInfo>
#include "handlemesserror.h"
#include <QDebug>
#include <QRegExp>

parserFileTab::parserFileTab(const QString &vFileName)
{
    QFileInfo info(vFileName);
    fileName=vFileName;
    fileName.replace(info.suffix(),"TAB");
    QFile file(fileName);
    context="Загрузка фото";
    if(!file.exists())
    {
        QString strMess=QString("Не найден файл %1").arg(fileName);
        throw handleMessError(strMess,context);
    }


}

QList<QPointF> parserFileTab::getCoordRaster()
{
    QList<QPointF> list;
    QFile file(fileName);
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
          {
              QString strMess=QString("Ошибка открытия файла %1 на чтение").arg(fileName);
              throw handleMessError(strMess,context);
          }
          bool isFind=false;
//          int i=0;
          while (!file.atEnd()&& (list.count()<4)) {
              QString line = file.readLine();
              if(isFind)
                  list.append(getCoord(line.trimmed()));
              if(line.trimmed()=="Type \"RASTER\"")
                  isFind=true;

          }
     return list;
}

QPointF parserFileTab::getCoord(QString str)
{
    QString strMess=QString("Неверный формат файла %1. Не найдены координаты").arg(fileName);
    QRegExp rx("\\(*\\)");
    if(rx.indexIn(str)==-1)
                throw handleMessError(strMess,context);
    str=str.mid(1,rx.indexIn(str)-1);
    rx.setPattern(",");
    int n=rx.indexIn(str);
    if(n==-1)
                throw handleMessError(strMess,context);
    QPointF p;
    bool ok;
    p.setX(str.mid(n+1).toFloat(&ok));
    if(!ok)
        throw handleMessError(strMess,context);
    p.setY(str.mid(0,n).toFloat(&ok));
    if(!ok)
        throw handleMessError(strMess,context);
    return p;
}
