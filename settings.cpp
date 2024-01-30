#include "settings.h"
#include <QMessageBox>
#include <QApplication>
#include "handlemesserror.h"
#include "dbworker.h"

Settings::Settings()
{
    QString pathIni=createTmpIniFile();
    //qDebug()<<pathIni;
    QSettings *settings;
    settings=new QSettings(pathIni,QSettings::IniFormat);

    appName = "Поиск нарушений землепользования";
    WidthBorder=settings->value("Width/border",5).toString();
    calcError=settings->value("error/calcError",5).toString();
    minSq=settings->value("out/minSq",6).toString();
    part=settings->value("out/part",10).toString();

    pathNeuralNetHouse=settings->value("exe/netHouse").toString();

    pathDB=QFileInfo(QApplication::applicationFilePath()).path();
    pathDB=QString("%1/").arg(settings->value("db/dir",pathDB).toString());


    colorSq = settings->value("lineParams/colorSq","yellow").toString();
    colorError = settings->value("lineParams/colorError","green").toString();
    tmplReportVneIZHSPath = settings->value("report/templateVneIZHS").toString();
    tmpltDopZahvatPath = settings->value("report/templateDopZahvat").toString();

    widthArea=settings->value("areaSize/width",0).toUInt();
    heightArea=settings->value("areaSize/height",0).toUInt();
}


QString Settings::getPathDB()
{
      return pathDB;
}

bool Settings::createConnection()
{
   try
    {
        QString context="Соединение с базой данных";
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");

        QString pathDb=QString("%1%2db_atlas.sqlite").arg(dir).arg(QDir::separator());

        QFile file(pathDb);
        if (!file.exists())
        {
            QString pathDb_null=QString("%1%2db_atlas.sqlite").arg(QCoreApplication::applicationDirPath())
                                                              .arg(QDir::separator());
            QFile file_null(pathDb_null);
            if (!file_null.exists())
            {
                 throw  handleMessError( QString("Не найден файл %1").arg(pathDb_null),context);

            }
            if(!file_null.copy(pathDb))
            {
                 throw  handleMessError( QString("Ошибка копирования файла %1 в %2").arg(pathDb_null).arg(pathDb),context);

            }

        }
        sdb.setDatabaseName(pathDb);
        if (!sdb.open())
            throw  handleMessError( sdb.lastError().text(),context);
        return true;
   }
    catch (handleMessError &e)
   {

        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
        return false;
    }
}


void Settings::setDir(QString fileName)
{   

   QFileInfo info(fileName);

   if(info.suffix()==suffix_atl)
   {
       isDirValid=true;

       QString name_photo=info.baseName();

       dir=QString("%1%2%3").arg(info.absolutePath()).arg(QDir::separator()).arg(name_photo);
       if (createConnection())
       {
           dbWorker dbW;
           idPhoto=dbW.getIdPhoto(name_photo);
       }

   }
   else
   {
       QString strMess="Не найден файл *.atl";
       QString context="Открытие файла";
       throw handleMessError(strMess,context);
   }
}

QString Settings::getDir()
{
    return dir;
}

bool Settings::dirValid()
{
    return isDirValid;
}

QString Settings::getFilePhoto()
{
    QString fileName=QString("%1%2%3").arg(dir).arg(QDir::separator()).arg("p.jpg");
    QFileInfo info(fileName);
    if (!info.exists())
    {
        QString strMess=QString("Не найден файл %1").arg(fileName);
        QString context="Открытие файла";
        throw handleMessError(strMess,context);
    }
    return fileName;
}

QString Settings::getFileBorders()
{
    return QString("%1%2%3").arg(dir).arg(QDir::separator()).arg("fb1.png");

}

QString Settings::getFileCMap()
{
    auto fName = QString("%1%2%3").arg(dir).arg(QDir::separator()).arg("cb.png");
    if (!(QFile(fName).exists())) {
      throw handleMessError(QString("Не найден файл %1").arg(fName), "Открытие файла-маски");
    }

    return fName;
}

QString Settings::getFileBuildings()
{
    return QString("%1%2%3").arg(dir).arg(QDir::separator()).arg("bb1.png");
}

QString Settings::getDirXml()
{
    qDebug()<<QString("%1%2XML").arg(dir).arg(QDir::separator());
    return QString("%1%2XML").arg(dir).arg(QDir::separator());
}

QString Settings::createTmpIniFile()
{
    QString pathIniTmp=QDir::homePath() +  "/controlland.ini";
    QString pathIni=QCoreApplication::applicationDirPath()+"/controlland.ini";
    qDebug()<<"createTmpIniFile"<<pathIni;

    QFile file(pathIni);
    if (!file.exists())
    {
        QMessageBox::warning(0,
                           appName,
                           QString("Не найден файл настроек %1").arg(pathIni),
                           QMessageBox::Ok);
        return pathIniTmp;

    }
    QFile fileTmp(pathIniTmp);
    if (file.open(QIODevice::ReadOnly)&&fileTmp.open(QIODevice::WriteOnly))
    {
        QString str=QString(file.readAll());
        str.replace("\\","/");
        QTextStream out(&fileTmp);
        out<<str;
        fileTmp.close();
    }
    else
    {
     if(!file.open(QIODevice::ReadOnly)) qDebug()<<QString("Ошибка чтения файла %1").arg(pathIni);
     if(!fileTmp.open(QIODevice::WriteOnly)) qDebug()<<QString("Ошибка записи в файл %1").arg(pathIniTmp);
    }
    return pathIniTmp;
}


qreal Settings::getWidthLine()
{
    return WidthBorder.toDouble();
}

qreal Settings::getCalcError()
{
      return calcError.toDouble();
}

qreal Settings::getMinSq()
{
      return minSq.toDouble();
}


QString Settings::getAppName()
{
      return appName;
}

int Settings::getPart()
{
    return part.toInt();
}


QString Settings::getSquareLineColor()
{
    return colorSq;
}


QString Settings::getErrorLineColor()
{
    return colorError;
}

uint Settings::getWidthArea()
{
   return widthArea;
}

uint Settings::getHeightArea()
{

        return heightArea;
}

QString Settings::getTmplReportVneIZHS()
{
    return tmplReportVneIZHSPath;
}

QString Settings::getTmplReportDopZahvat()
{
    return tmpltDopZahvatPath;

}


QString Settings::getPathNeuralNetHouse()
{
    return pathNeuralNetHouse;
}


int Settings::getIdPhoto()
{   
    //if(idPhoto == 0)
      //  throw  handleMessError("Не существует идентификатора фото","Чтение данных");
    return idPhoto;
}


