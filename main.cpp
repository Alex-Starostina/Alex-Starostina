#include "mainwindow.h"
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "settings.h"
#include <QDebug>
#include "handlemesserror.h"
#include <QMessageBox>
#include <QtSql>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //локал-я приложения
    QString translationsPath(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QLocale locale = QLocale::system();

    QTranslator qtTranslator;
    if (qtTranslator.load(locale, "qt", "_", translationsPath))
       a.installTranslator(&qtTranslator);

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load(locale, "qtbase", "_", translationsPath))
       a.installTranslator(&qtBaseTranslator);


   MainWindow w;
   w.show();
   if(argc>1)
   {
       try {
           Settings::Instance().setDir(QString::fromLocal8Bit(argv[1]));
           w.loadPhoto();
       }
       catch (handleMessError &e)
       {
            QMessageBox::warning(0,
                                   Settings::Instance().getAppName(),
                                   QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                                   QMessageBox::Ok);
            return 1;
       }
   }
   return a.exec();
}
