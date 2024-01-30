#include "reportgenerator.h"
#include "settings.h"
#include <QDir>
#include <QAxObject>
#include <QDebug>
#include <QMessageBox>
#include <QDate>

ReportGenerator::ReportGenerator(QObject *parent ) :  QObject(parent) { }


//замена подстроки в шаблоне отчета
void ReportGenerator::replaceStrInReport(const QString& oldStr, const QString& newStr, QAxObject *wordApp)
{
    QAxObject *activeWnd = wordApp->querySubObject("Activewindow");
    if (!activeWnd) return;
    QAxObject* selection = activeWnd->querySubObject("Selection");
    QAxObject* findObj = selection->querySubObject("Find");
    if (!findObj) return;
    QList<QVariant> params = { QVariant(oldStr), QVariant("0"), QVariant("0"), QVariant("0"), QVariant("0"), QVariant("0"),
                             QVariant(true), QVariant("0"), QVariant("0"), QVariant(newStr), QVariant("2"), QVariant("0"),
                             QVariant("0"), QVariant("0"), QVariant("0") };
    findObj->dynamicCall("Execute(const QVariant&,const QVariant&,const QVariant&,const QVariant&,const QVariant&,"
                      "const QVariant&,const QVariant&,const QVariant&,const QVariant&,const QVariant&,"
                      "const QVariant&,const QVariant&,const QVariant&,const QVariant&,const QVariant&)", params);

    delete findObj;
    delete selection;
    delete activeWnd;
}



// табличный отчет с информацией и фото объектов недвиж-ти
void ReportGenerator::reportTableWithPhoto(QMap<QString, estate_Rec> listObjs)
{
    QString fileName;
    estate_Rec rec1;

    QAxObject *wordApp=new QAxObject("Word.Application");
    if (wordApp == nullptr) {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не найден установленный экземпляр Ms Word"));
        return;
    }

    wordApp->setProperty("Visible",true);
    QAxObject *wordDocs=wordApp->querySubObject("Documents()");
    if (wordDocs == nullptr) {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не найдена коллекция документов Ms Word"));
        return;
    }

    wordDocs->querySubObject("Add()");
    QAxObject *activeDoc=wordApp->querySubObject("ActiveDocument()");
    QAxObject *pSelection=activeDoc->querySubObject("Activewindow")->querySubObject("selection");
    QAxObject *pTables=activeDoc->querySubObject("Tables()");
    QAxObject *pNewTable=pTables->querySubObject("Add(Range,NumRows,NumColumns)", pSelection->property("Range"), 1, 2);
    //рамка вокруг табл-ы
    QAxObject *bb = pNewTable->querySubObject("Borders");
    bb->setProperty("OutsideLineStyle", QVariant("wdLineStyleDouble"));
    bb->setProperty("InsideLineStyle", QVariant("wdLineStyleSingle"));

    QAxObject *pCell=NULL, *pCellRange=NULL;
    int cur_row=1;
    QAxObject *pCurRow, *inlineShapes;

    QMapIterator<QString, estate_Rec> iter(listObjs);
    while(iter.hasNext())
    {
        iter.next();
        fileName = iter.key();
        qDebug() << fileName;
        if (!QFile::exists(fileName))
          qDebug() << fileName << " не найден!!";

        rec1 = iter.value();

        pSelection->dynamicCall("InsertRowsBelow()");
        pCurRow=pNewTable->querySubObject("Rows(index)", cur_row);

        pCell=pCurRow->querySubObject("Cells(index)", 1);
        pCellRange=pCell->querySubObject("Range()");

        pCellRange->dynamicCall("InsertAfter(Text)", rec1.adr);

        pCell=pCurRow->querySubObject("Cells(index)", 2);
        pCellRange=pCell->querySubObject("Range()");

//  qDebug() << "fileNam " << fileName;
        if (pCellRange != nullptr) {
          inlineShapes = pCellRange ->querySubObject("InlineShapes");
//     connect(inlineShapes, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));
          if (inlineShapes != nullptr)  inlineShapes ->querySubObject("AddPicture(const QString&)", fileName);
        }
        cur_row++;
    }
//    activeDoc->dynamicCall("Close()" );
    wordApp->dynamicCall("Quit(void)");
//    QMessageBox::information(nullptr, Settings::Instance().getAppName(), QString("Таблица с фото нарушений готова!"));
}


//отчет о здании вне ИЖС
void ReportGenerator::doReportVneIzhS(QString scrShot, estate_Rec realEstateRec)
{
    QString fileName="otch_obj_vne_izhs_templ2.docx";
    QString tmplStrOld = QString("%1/shablony/%2").arg(QCoreApplication::applicationDirPath()).arg(fileName);
    QString folderStr = Settings::Instance().getDir().append("/reports");
    QDir dir;

    if (!dir.exists(folderStr))
        dir.mkdir(folderStr);

    QString tmplStr = folderStr + "/"+ fileName;
    if (!QFile(tmplStrOld).exists()) {
      QMessageBox::warning(0, Settings::Instance().getAppName(),
                           QString("Не найден шаблон отчета для территории вне ИЖС ").append(tmplStrOld));
      return;
    }


    QFile(tmplStrOld).copy(tmplStr);
    QString nowDt = QDate().currentDate().toString("dd.MM.yyyy");
//    qDebug()<<tmplStr;


    QAxObject *wordApp = new QAxObject( "Word.Application", this );
    if (wordApp == nullptr)  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не найден экземпляр Microsoft Word"));
        return;
    }

    QAxObject *docs = wordApp->querySubObject( "Documents");
    if (docs == nullptr) {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не удается открыть коллекцию документов Ms Word"));
        return;
    }

    QVariant tmpl(tmplStr);
    QAxObject *doc = docs->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&, const QVariant&,"
                                           "const QVariant&, const QVariant&, const QVariant&)", tmpl, QVariant(false),
                                        QVariant(false), QVariant(false), QVariant(""), QVariant(""), QVariant(false));
    connect(doc, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));
    if ( doc == nullptr)  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString( "Не открывается шаблон отчета %1").arg(tmplStr));
        return;
    }

    QAxObject *activeDoc = wordApp->querySubObject( "ActiveDocument");
    if (activeDoc == nullptr) return;

    QAxObject *bMarks = activeDoc->querySubObject("Bookmarks()");
    if (bMarks != nullptr) {
        QAxObject *bm1 = bMarks->querySubObject("Item(Name)", "img1");
        if (bm1 != nullptr) {
            QAxObject *pictRng = bm1->querySubObject("Range");
            QAxObject *inlineShps = activeDoc->querySubObject("InlineShapes");
            if (inlineShps != nullptr) {
                if (QFile(scrShot).exists()) {
                  inlineShps->querySubObject("AddPicture(FileName,LinkToFile,SaveWithDocument,Range)", scrShot,
                                                   QVariant(false), QVariant(true), pictRng->asVariant());
 //               connect(newShp, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));
                }
            }

          delete inlineShps;
          delete pictRng;
        }

      delete bm1;
    }

    replaceStrInReport("{dd.MM.yyyy}", nowDt, doc);
    replaceStrInReport("{adres}", realEstateRec.adr, doc);
    if(realEstateRec.info.isEmpty())
    {
        replaceStrInReport("{istochnik}", "---", doc);
        replaceStrInReport("{type_obj}", "----", doc);
    }
    else
    {
        replaceStrInReport("{istochnik}", "2ГИС", doc);
        replaceStrInReport("{type_obj}", realEstateRec.info, doc);

    }
    wordApp->setProperty("Visible",true);
    doc->dynamicCall("SaveAs2(const QVariant&)", folderStr.append("/otchVneIzhs.doc"));
    wordApp->dynamicCall("Quit(void)");
    QMessageBox::information(0, Settings::Instance().getAppName(),
                             QString("Отчет reports/otchVneIzhs.doc для объекта находящегося вне зоны ИЖС готов."));
}

//отчет о выходе строения за кадастр. границу
void ReportGenerator::doReportCadastrTerr(QString scrShot, estate_Rec realEstateRec)
{
    QString fileName="tmpl_dop_zahvat_terr.docx";
    QString tmplStrOld = QString("%1/shablony/%2").arg(QCoreApplication::applicationDirPath()).arg(fileName);
    QString folderStr = Settings::Instance().getDir().append("/reports");

    QDir dir;
//qDebug() << "tmplStrO " << tmplStrOld;

    if (!dir.exists(folderStr))
        dir.mkdir(folderStr);

    QString tmplStr = folderStr + "/"+ fileName;
    if (!QFile(tmplStrOld).exists()) {
      QMessageBox::warning(0, Settings::Instance().getAppName(),
                           QString("Не найден шаблон отчета доп. захвата территории ").append(tmplStrOld));
      return;
    }

    QFile(tmplStrOld).copy(tmplStr);
//qDebug() << "tmplStr " << tmplStr;
    QString nowDt = QDate().currentDate().toString("dd.MM.yyyy");

    QAxObject *wordApp = new QAxObject( "Word.Application", this );
    if (wordApp == nullptr)  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("На компьютере не найден экземпляр Microsoft Word"));
        return;
    }

    QAxObject *docs = wordApp->querySubObject( "Documents");
    if (docs == nullptr) {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не удается открыть коллекцию документов Ms Word"));
        return;
    }

    QVariant tmpl(tmplStr);
    QAxObject *doc = docs->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&, const QVariant&,"
                                           "const QVariant&, const QVariant&, const QVariant&)", tmpl, QVariant(false),
                                        QVariant(false), QVariant(false), QVariant(""), QVariant(""), QVariant(false));
    connect(doc, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));

    if ( doc == nullptr)  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не открывается шаблон отчета %1").arg(tmplStr));
        return;
    }

    QAxObject *activeDoc = wordApp->querySubObject( "ActiveDocument");
    if (activeDoc == nullptr) return;

    QAxObject *bMarks = activeDoc->querySubObject("Bookmarks()");
    if (bMarks != nullptr) {
        QAxObject *bm1 = bMarks->querySubObject("Item(Name)", "img1");
        if (bm1 != nullptr) {
          QAxObject *pictRng = bm1->querySubObject("Range");
          QAxObject *inlineShps = activeDoc->querySubObject("InlineShapes");

             if (inlineShps != nullptr) {
                 if (QFile(scrShot).exists()) {
                   inlineShps->querySubObject("AddPicture(FileName,LinkToFile,SaveWithDocument,Range)", scrShot,
                                                    QVariant(false), QVariant(true), pictRng->asVariant());
                 }
             }

             delete inlineShps;
             delete pictRng;
        }

        delete bm1;
    }

    replaceStrInReport("{dd.MM.yyyy}", nowDt, doc);
    replaceStrInReport("{adres}",realEstateRec.adr, doc);
    replaceStrInReport("{KN}", realEstateRec.kadNum, doc);


    if(realEstateRec.info.isEmpty())
    {
        replaceStrInReport("{istochnik}", "---", doc);
        replaceStrInReport("{type_obj}", "----", doc);
    }
    else
    {
        replaceStrInReport("{istochnik}", "2ГИС", doc);
        replaceStrInReport("{type_obj}", realEstateRec.info, doc);

    }


    wordApp->setProperty("Visible",true);
    doc->dynamicCall("SaveAs2(const QVariant&)", folderStr.append("/otchDopZahvat.doc"));
    wordApp->dynamicCall("Quit(void)");

    QMessageBox::information(0, Settings::Instance().getAppName(),
                             QString("Отчет reports/otchDopZahvat.doc 'Выход за кадастровую границу' готов."));
}


void ReportGenerator::printError(int errCod, QString src, QString desc, QString helpTxt)
{
    Q_UNUSED(helpTxt);
    qDebug() << "Код: " << errCod << " Источник ошибки: " << src << " Описание: " << desc;
}




void ReportGenerator::doReport(QString fileName,QString scrShot,estate_Rec realEstateRec)
{
    QString tmplStr = QString("%1/shablony/%2").arg(QCoreApplication::applicationDirPath()).arg(fileName);

    QString folderStr = Settings::Instance().getDir().append("/reports");

    QDir dir;

    if (!dir.exists(folderStr))
        dir.mkdir(folderStr);

    QString tmplStr_copy = QString("%1/%2").arg(folderStr).arg(fileName);
    QFile(tmplStr).copy(tmplStr_copy);
    QString nowDt = QDate().currentDate().toString("dd.MM.yyyy");

    //qDebug()<<tmplStr;
    if (!QFile(tmplStr_copy).exists()) {
      QMessageBox::warning(0, Settings::Instance().getAppName(),
                           QString("Не найден шаблон отчета  %1").arg(tmplStr));
      return;
    }

    QAxObject *wordApp = new QAxObject( "Word.Application", this );
    if (wordApp == nullptr)  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не найден экземпляр Microsoft Word"));
        return;
    }

    QAxObject *docs = wordApp->querySubObject( "Documents");
    if (docs == nullptr) {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не удается открыть коллекцию документов Ms Word"));
        return;
    }

    QVariant tmpl(tmplStr);
    QAxObject *doc = docs->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&, const QVariant&,"
                                           "const QVariant&, const QVariant&, const QVariant&)", tmpl, QVariant(false),
                                        QVariant(false), QVariant(false), QVariant(""), QVariant(""), QVariant(false));
    connect(doc, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));
    if ( !doc )  {
        QMessageBox::warning(0, Settings::Instance().getAppName(), QString( "Не открывается шаблон отчета %1").arg(tmplStr));
        return;
    }

    QAxObject *activeDoc = wordApp->querySubObject( "ActiveDocument");
    if (activeDoc == nullptr) return;

    QAxObject *bMarks = activeDoc->querySubObject("Bookmarks()");
    if (bMarks != nullptr) {
        QAxObject *bm1 = bMarks->querySubObject("Item(Name)", "img1");
        if (bm1 != nullptr) {
            QAxObject *pictRng = bm1->querySubObject("Range");
            QAxObject *inlineShps = activeDoc->querySubObject("InlineShapes");
            if (inlineShps != nullptr) {
                if (QFile(scrShot).exists()) {
                  inlineShps->querySubObject("AddPicture(FileName,LinkToFile,SaveWithDocument,Range)", scrShot,
                                                   QVariant(false), QVariant(true), pictRng->asVariant());
                }
            }

          delete inlineShps;
          delete pictRng;
        }

      delete bm1;
    }

    replaceStrInReport("{dd.MM.yyyy}", nowDt, doc);
    replaceStrInReport("{adres}", realEstateRec.adr, doc);
    replaceStrInReport("{KN}", realEstateRec.kadNum, doc);
    if(realEstateRec.info.isEmpty())
    {
        replaceStrInReport("{istochnik}", "---", doc);
        replaceStrInReport("{type_obj}", "----", doc);
    }
    else
    {
        replaceStrInReport("{istochnik}", "2ГИС", doc);
        replaceStrInReport("{type_obj}", realEstateRec.info, doc);

    }
    QMessageBox::information(0, Settings::Instance().getAppName(),
                              QString("Отчет готов."));
    wordApp->setProperty("Visible",true);
    //doc->dynamicCall("SaveAs2(const QVariant&)", dirUp.path().append("/otchVneIzhs.doc"));
    wordApp->dynamicCall("Quit(void)");

}

//void ReportGenerator::doReport(QString fileName, QString scrShot, estate_Rec realEstateRec)
//{
//    QString tmplStr = QString("%1/shablony/%2").arg(QCoreApplication::applicationDirPath()).arg(fileName);
//    QString folderStr = Settings::Instance().getDir().append("/reports");
//    QDir dir;

//    if (!dir.exists(folderStr))
//        dir.mkdir(folderStr);

//    QString tmplStr_copy = QString("%1/%2").arg(folderStr).arg(fileName);
//    QFile(tmplStr).copy(tmplStr_copy);
//    QString nowDt = QDate().currentDate().toString("dd.MM.yyyy");

//    qDebug()<<tmplStr;
//    if (!QFile(tmplStr).exists()) {
//      QMessageBox::warning(0, Settings::Instance().getAppName(),
//                           QString("Не найден шаблон отчета  %1").arg(tmplStr));
//      return;
//    }

//    QAxObject *wordApp = new QAxObject( "Word.Application", this );
//    if (wordApp == nullptr)  {
//        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не найден экземпляр Microsoft Word"));
//        return;
//    }

//    QAxObject *docs = wordApp->querySubObject( "Documents");
//    if (docs == nullptr) {
//        QMessageBox::warning(0, Settings::Instance().getAppName(), QString("Не удается открыть коллекцию документов Ms Word"));
//        return;
//    }

//    QVariant tmpl(tmplStr);
//    QAxObject *doc = docs->querySubObject("Open(const QVariant&, const QVariant&, const QVariant&, const QVariant&,"
//                                           "const QVariant&, const QVariant&, const QVariant&)", tmpl, QVariant(false),
//                                        QVariant(false), QVariant(false), QVariant(""), QVariant(""), QVariant(false));
//    connect(doc, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(printError(int,QString,QString,QString)));
//    if (doc == nullptr)  {
//        QMessageBox::warning(0, Settings::Instance().getAppName(), QString( "Не открывается шаблон отчета %1").arg(tmplStr));
//        return;
//    }

//    QAxObject *activeDoc = wordApp->querySubObject( "ActiveDocument");
//    if (activeDoc == nullptr) return;

//    QAxObject *bMarks = activeDoc->querySubObject("Bookmarks()");
//    if (bMarks != nullptr) {
//        QAxObject *bm1 = bMarks->querySubObject("Item(Name)", "img1");
//        if (bm1 != nullptr) {
//            QAxObject *pictRng = bm1->querySubObject("Range");
//            QAxObject *inlineShps = activeDoc->querySubObject("InlineShapes");
//            if (inlineShps != nullptr) {
//                if (QFile(scrShot).exists()) {
//                  inlineShps->querySubObject("AddPicture(FileName,LinkToFile,SaveWithDocument,Range)", scrShot,
//                                                   QVariant(false), QVariant(true), pictRng->asVariant());
//                }
//            }

//          delete inlineShps;
//          delete pictRng;
//        }

//      delete bm1;
//    }

//    replaceStrInReport("{dd.MM.yyyy}", nowDt, doc);
//    replaceStrInReport("{adres}", realEstateRec.adr, doc);
//    replaceStrInReport("{KN}", realEstateRec.kadNum, doc);
//    if(realEstateRec.info.isEmpty())
//    {
//        replaceStrInReport("{istochnik}", "---", doc);
//        replaceStrInReport("{type_obj}", "----", doc);
//    }
//    else
//    {
//        replaceStrInReport("{istochnik}", "2ГИС", doc);
//        replaceStrInReport("{type_obj}", realEstateRec.info, doc);

//    }
//    QMessageBox::information(0, Settings::Instance().getAppName(),
//                              QString("Отчет готов."));
//    wordApp->setProperty("Visible",true);
//    //doc->dynamicCall("SaveAs2(const QVariant&)", dirUp.path().append("/otchVneIzhs.doc"));
//    wordApp->dynamicCall("Quit(void)");

//}

