#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAxObject>
#include <ActiveQt/qaxbase.h>
#include "domparser.h"
#include "settings.h"
#include <QProgressDialog>
#include "handlemesserror.h"
#include "parserfiletab.h"
#include "math.h"
#include "atlaspainter.h"
//#include "ui_formborder.h"
#include "dbworker.h"
#include "borderobj.h"
#include "reportgenerator.h"
#include "splashscreen.h"
#include <QWidget>
#include <QMap>
#include <QFile>
#include <QTextEdit>
#include <QAction>
#include <math.h>
#include "readerparcelsjson.h"

//-------------------------------------MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , form_violation(nullptr), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->tabViolation->setCurrentIndex(0);
    //sel_tv_violation_index = -1;
    fillStartInfo();
    this->setWindowTitle(Settings::Instance().getAppName());
    QStringList labels;
    labels<<"Кадастровый №"<<"Адрес"<<"Тип";

    ui->treeWidget->setHeaderLabels(labels);
    ui->treeWidget->setColumnCount(labels.count());
    ui->graphicsViewTif->setDragMode(QGraphicsView::QGraphicsView::ScrollHandDrag);

    //белый бордюр вокруг фотоснимка местности
    ui->graphicsViewTif->setStyleSheet("border: 10px solid white; ");
    ui->graphicsViewTif->setScene(&sceneTif);

    glPosContextMenu.setX(0);
    glPosContextMenu.setY(0);
    init_menu();
    readSettings();

    modelViolation = new ViolationSqlModel;
    ui->tv_violation->setModel(modelViolation);
    ui->tv_violation->setColumnHidden(0,true);
    ui->tv_violation->setColumnHidden(4,true);
    ui->tv_violation->horizontalHeader()->setVisible(true);
    ui->tv_violation->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tv_violation,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(showFormViolation(const QModelIndex &)));
    connect(ui->tv_violation,SIGNAL(customContextMenuRequested ( const QPoint & )),this,
            SLOT(showContexMenu_violation(const QPoint & )));

    connect(ui->tv_violation, SIGNAL(clicked(const QModelIndex &)), this,
            SLOT(tv_violation_clicked(const QModelIndex &)));
    connect(modelViolation,SIGNAL(deleteBorderFromModel(int)),this,SLOT(deleteBorder(int)));

    popupMenu_Violation= new QMenu(this);
    popupMenu_Violation->addAction(ui->actionUpdateViolation);
    popupMenu_Violation->addAction(ui->actionReport);

    popupMenu_Violation->addAction(ui->actionDeleteViolation);

    //ui->tabViolation->setTabText(3,"Вне ИЖС");
    //ui->tabViolation->setTabText(4,"Пересечение");
    ui->tabViolation->removeTab(3);
    ui->tabViolation->removeTab(3);
    //ui->btnPaintAll->hide();
    //ui->btnClear->hide();
    //ui->btnLoad->hide();
}


void MainWindow::fillStartInfo()
{
    QString c3 = Settings::Instance().getSquareLineColor();
    QString c5 = Settings::Instance().getErrorLineColor();

    QString c1 = "blue", c2 = "red", style1 = QString("border-style: solid;border-width: 2px;border-color: %1;").arg(c1),
             style2 = QString("border-style: solid;border-width: 2px;border-color: %1;").arg(c2),
             style3 = QString("border-style: solid;border-width: 2px;border-color: %1;").arg(c3),
             style5 = QString("border-style: solid;border-width: 2px;border-color: %1;").arg(c5);

    ui->lgdLabel1->setStyleSheet( style1);
    ui->lgdLabel1->setText("");
    ui->lgdLabel1->setFixedSize(35, 15);
    ui->lgdLabel2->setStyleSheet( style2);
    ui->lgdLabel2->setText("");
    ui->lgdLabel2->setFixedSize(35, 15);

    ui->lgdLabel3->setStyleSheet( style3);
    ui->lgdLabel3->setText("");
    ui->lgdLabel3->setFixedSize(35, 15);
    ui->lgdLabel5->setStyleSheet( style5);
    ui->lgdLabel5->setText("");
    ui->lgdLabel5->setFixedSize(35, 15);

    QPixmap pixmap1;
    pixmap1.load( ":/images/circle.png");
    ui->lgdLabel4->setPixmap(pixmap1);
}


void  MainWindow::showFormViolation(const QModelIndex &index)
{

    if(form_violation!=nullptr &&form_violation->isModified())
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             "Закройте панель нарушения",
                             QMessageBox::Ok);
        return;
    }
    if(form_violation!=nullptr)
    {

        form_violation->on_btnCancel_clicked();

    }
    int id=modelViolation->getId(index);
    int typeV=modelViolation->getTypeViolation(index);
    form_violation=new FormDBViolation(&sceneTif,noMode,id,typeV);
    connect(form_violation,SIGNAL(closeViolation(bool)),this,SLOT(deleteFormViolation(bool)));
    ui->tab_5->layout()->addWidget(form_violation);
    ui->widget->hide();
    ui->tabViolation->setCurrentIndex(0);
    //ui->tv_violation->resizeRowsToContents();
    //ui->tv_violation->resizeColumnsToContents();
    QPointF p=form_violation->centerMap();
    if(!p.toPoint().isNull())
    {
        ui->graphicsViewTif->centerOn(p);
    }
}

void MainWindow::deleteFormViolation(bool refresh)
{
    if(form_violation!=nullptr)
    {
        disconnect(form_violation,SIGNAL(closeViolation(bool)),this,SLOT(deleteFormViolation(bool)));
        //disconnect(form_violation,SIGNAL(insertViolationDB()),modelViolation,SLOT(init()));
        form_violation->deleteLater();
        form_violation=nullptr;
        ui->widget->show();
        if(refresh)
        {
            modelViolation->init();
            sceneTif.updateListPolygon();
            ui->tv_violation->setColumnHidden(0,true);
            ui->tv_violation->setColumnHidden(4,true);

        }
    }
}

void MainWindow::showContexMenu_violation(const QPoint &pos)
{
    QModelIndex index = ui->tv_violation->selectionModel()->currentIndex();
    if (!index.isValid()) return;
    QPoint p=ui->tv_violation->mapToGlobal(pos);
    p.setY(p.y()+ui->tv_violation->horizontalHeader()->size().height());
    popupMenu_Violation->exec(p);
}

void MainWindow::tv_violation_clicked(const QModelIndex &ind)
{


    QPointF centerP =  modelViolation->getCenter(ind);
    if (!centerP.isNull())
        ui->graphicsViewTif->centerOn(centerP);
}


void MainWindow::deleteBorder(int id)
{
    dbBorderObj *border=new dbBorderObj(&sceneTif,id);
    border->deleteDB();
    border->deleteFromScene();
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
        case Qt::Key_Escape:          
          sceneTif.deleteLastPoint();
          break;
        default:
           QWidget::keyPressEvent(e);    
    }
}

void MainWindow::on_btnLoad_clicked()
{
    QFileDialog *fileDlg = new QFileDialog(this, Qt::Dialog);
    QString dir = fileDlg->getExistingDirectory();
    if (dir != "")
    {
        loadXML(dir);
    }
}

void MainWindow::loadXMLFromDir()
{
    if (!Settings::Instance().dirValid()) return;
    QString dir = Settings::Instance().getDirXml();
    loadXML(dir);
}

void MainWindow::loadXML(QString dir)
{
    //получение списка файлов выбран. директории
    QFileInfo fInfo;
    QDir selDir(dir);
    QFileInfoList list = selDir.entryInfoList();
    if (list.size()>0)
    {
      ui->treeWidget->clear();
      numPlot=1;

    }

    if (sceneTif.img.isNull()) {
        showMessage("Не загружен аэрофотоснимок, данные кадастрового учета загружены не будут!");
        return;
    }

    DomParser parser(ui->treeWidget,&sceneTif);

    for (int i = 0; i < list.size(); i++) {
      fInfo = list.at(i);
      if (fInfo.suffix().toLower() == "xml") {
          parser.readFile(fInfo.absoluteFilePath());
      }
    }

}

void MainWindow::showContextMenu(const QPoint& pos)
{
    Q_UNUSED(pos);

    // получаем глобальные координаты
//    glPosContextMenu = pos;
//            //ui->graphicsViewTif->mapToGlobal(pos);
}


//чтение настроек приложения
void MainWindow::readSettings()
{
    QSettings settings("Mars","controlland");
    this->restoreGeometry(settings.value("geometry").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    writeSettings();
}

//сохранение настроек приложения
void MainWindow::writeSettings()
{
    QSettings settings("Mars","controlland");
    settings.setValue("geometry",saveGeometry());
}


void MainWindow::centerOnView(QListWidgetItem *item)
{
   QList<QVariant> pointsArea=item->data(Qt::UserRole).toList();

   ui->graphicsViewTif->centerOn(pointsArea.first().toPoint());
}

void MainWindow::init_menu()
{
    //добавл-е контекстного меню
    ui->graphicsViewTif->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->graphicsViewTif, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    ui->fileMenu->addAction(ui->actionOpenFile);

//    ui->actionMenu->addAction(ui->actionAddSqPoints);
    ui->actionMenu->addAction(ui->calculateSquare);
    ui->actionMenu->addAction(ui->meterWidth);

    ui->actionMenu->addSeparator();

    ui->actionMenu->addAction(ui->actionAddMark);
    ui->actionMenu->addAction(ui->actionNext);
    ui->actionMenu->addAction(ui->actionGetNums);
    ui->actionMenu->addAction(ui->actionSetPoint);

    ui->actionMenu->addSeparator();
    ui->actionMenu->addAction(ui->clearMarkup);

    ui->otchetMenu->addAction(ui->actionReport);

    popupMenu= new QMenu(this);
    popupMenu->addAction(ui->actionAddMark);
    popupMenu->addAction(ui->actionSetPoint);
    popupMenu->addAction(ui->actionSelecterror);
    popupMenu->addSeparator();
    popupMenu->addAction(ui->clearMarkup);
    popupMenu->addSeparator();
    popupMenu->addAction(ui->calculateSquare);

    popupMenu->addAction(ui->meterWidth);

    popupMenu->addAction(ui->printInfo);    
    connect(ui->graphicsViewTif,SIGNAL(customContextMenuRequested ( const QPoint & )),this,
                        SLOT(tableContextMenuRequested(const QPoint & )));
}

void MainWindow::tableContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)

   /* QPoint p=ui->graphicsViewTif->mapToGlobal(pos);
    ui->printInfo->setEnabled(sceneTif.selObjects.count()>0);
    ui->calculateSquare->setEnabled(sceneTif.areaPoints.count()>0);
//    ui->clearMarkup->setEnabled(sceneTif.selObjects.count()>0||sceneTif.areaPoints.count()>0);
    popupMenu->exec(p);*/
}


MainWindow::~MainWindow()
{
    delete form_violation;

    delete ui;
}

void MainWindow::showMessage(QString mStr)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(mStr);
    msg.setWindowTitle( Settings::Instance().getAppName());
    msg.exec();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item )
{
    /*QGraphicsPolygonItem *figurePng=sceneTif.drawGeoPolygon(item->data(0,Qt::UserRole).toList(), Qt::yellow);
    ui->graphicsViewTif->centerOn(figurePng);*/
    QList<QVariant> listVariants;
    listVariants = item->data(0,Qt::UserRole).toList();
    if (listVariants.count())
    {
        ui->graphicsViewTif->centerOn(listVariants[0].toPointF());
    }


}

void MainWindow::on_btnPaintAll_clicked()
{
    paintAllCadastr();
    sceneTif.saveScene("g.png");
}


void MainWindow::paintAllCadastr()
{

    /*for(int i=0;i<ui->treeWidget->topLevelItemCount();i++)
    {

        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
        sceneTif.drawGeoPolygon(item->data(0,Qt::UserRole).toList(), Qt::blue);
        qApp->processEvents();
    }*/

}

void MainWindow::on_btnClear_clicked()
{
   // sceneTif.removeObjects();

}

void MainWindow::on_actionAddMark_triggered()
{

    try {
        if (form_violation==nullptr)
        {
          form_violation=new FormNewViolation(&sceneTif,selectOrientir);
          connect(form_violation,SIGNAL(closeViolation(bool)),this,SLOT(deleteFormViolation(bool)));
          ui->tab_5->layout()->addWidget(form_violation);
          ui->widget->hide();
        }
        form_violation->addNewMark(ui->treeWidget);
        ui->tabViolation->setCurrentIndex(0);
        sceneTif.setMode(selectOrientir);
    } catch (handleMessError &e)
    {
         QMessageBox::warning(0,
                                Settings::Instance().getAppName(),
                                QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                                QMessageBox::Ok);
    }
}

void MainWindow::on_actionSetPoint_triggered()
{

    addViolation(calculateArea);
    sceneTif.isStandardCursor = false;
}

void MainWindow::on_actionSelecterror_triggered()
{
    addViolation(selectError);
}

void MainWindow::addViolation(TModeScene mode_)
{
 try {
        if (form_violation==nullptr)
        {
          form_violation=new FormNewViolation(&sceneTif,mode_);
          connect(form_violation,SIGNAL(closeViolation(bool)),this,SLOT(deleteFormViolation(bool)));
          ui->tab_5->layout()->addWidget(form_violation);
          ui->widget->hide();
        }
        if(mode_==calculateArea and sceneTif.getMode()==selectError) //form_violation->mode==selectError)
        {
            QMessageBox::warning(0,
                                   Settings::Instance().getAppName(),
                                   "Закройте панель ложного срабатывания",
                                   QMessageBox::Ok);
            return;
        }
        if(mode_==selectError and  sceneTif.getMode()==calculateArea)

        {
            QMessageBox::warning(0,
                                   Settings::Instance().getAppName(),
                                   "Закройте панель нарушений",
                                   QMessageBox::Ok);
            return;
        }
       /* if(mode_==calculateArea or (mode_==selectError and form_violation->mode==selectOrientir))
        {
            form_violation->mode=mode_;
        } */

        form_violation->addNewBorder(mode_);
        ui->tabViolation->setCurrentIndex(0);
        ui->graphicsViewTif->viewport()->setCursor(Qt::PointingHandCursor);
        sceneTif.setMode(mode_);
    }
    catch (handleMessError &e)
    {
         QMessageBox::warning(0,
                                Settings::Instance().getAppName(),
                                QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                                QMessageBox::Ok);
    }
}

void MainWindow::on_calculateSquare_triggered()
{
    unsigned int squar = 0;
    int xCoord = 0, yCoord = 0;

    if (sceneTif.areaPoints().size() > 2) {
        QVector<QPoint> pointsSq = sceneTif.areaPoints();
        QPolygon polyg(pointsSq);

        QVectorIterator<QPoint> pointsIterator(pointsSq);
        QPoint p = pointsSq.at(0);
        QPoint lowestP = pointsSq.at(0); //опорная точка вывода таблички с площадью
        int maxY = pointsSq.at(0).y();

        while (pointsIterator.hasNext()) { //опред-е самой нижн. точки многоугольника
            p = pointsIterator.next();
            if ( p.y() > maxY) {
              maxY = p.y();
              lowestP = p;
            }
        }

        if (!polyg.isEmpty()) {
          squar = sceneTif.painter->calculateAreaOnMap(pointsSq);
          int lblWidth = QString("Площадь: %1 кв.м").arg(squar).length()*6+6; //ширина таблички с площадью
//          xCoord = rectPolyg.center().x() - 90;
//          yCoord = rectPolyg.topLeft().y() - 30;
          xCoord = lowestP.x() - lblWidth/2;
          yCoord = lowestP.y();

          sceneTif.addTextOnMap( QString("Площадь: %1 кв.м").arg(squar),
                      QFont("Arial", 8, QFont::Bold),
                      QRect(xCoord, yCoord, lblWidth, 16),
                      QPoint(xCoord, yCoord - 3) );
        }

//      auto sqMap = sceneTif.areaPoints;
//      squar = sceneTif.calculateAreaOnMap(sqMap);

//      QMapIterator<int, QVector<QPoint>> iter(sqMap);

//      while (iter.hasNext() ) {
//        iter.next();
//        squar = 0;
//        if (iter.key() > 0)
//            squar = sceneTif.calculateAreaOnMap(iter.value());

//        if (squar > 0) {
//            ui->textEdit->append(QString("Площадь %1 участка: %2 кв. м").arg(iter.key()).arg(squar));
//            ui->textEdit->append("");
//            ui->tabWidgetTree->setCurrentIndex(2);
//        }
//      }

    } else
          showMessage("Не указан участок для расчета площади");
}

void MainWindow::on_meterWidth_triggered()
{
    QLine side;
    float  l;               //длина стороны участка
    uint heightNote = 15;   //высота сноски
    AtlasPainter *p = new AtlasPainter(&sceneTif);

   if (/*!sceneTif.areaPoints().isEmpty() &&*/ sceneTif.areaPoints().size() > 2) {

      //коорд-ы рассчитыв. участка
      QVector<QPoint> pointsSq = sceneTif.areaPoints();

      QRect boundRect = QPolygon(pointsSq).boundingRect();

      //Закрашиваю область участка
      //sceneTif.addPolygon(QPolygon(pointsSq),QPen(QColor(125,255,255,70)), QBrush(QColor(125,255,255,70))); //бирюзовая закраска
      sceneTif.addPolygon(QPolygon(pointsSq),QPen(QColor(255,255,0,70)), QBrush(QColor(255,255,0,70)));     //желтая закраска
      //sceneTif.addPolygon(QPolygon(pointsSq),QPen(QColor(38,240,120,70)), QBrush(QColor(38,240,120,70)));   //зеленая закраска

      for (int i = 0; i < pointsSq.size(); i++) {
        //длина стороны участка
        if (i == pointsSq.size()-1) {
            l = sceneTif.painter->meterWidthOfLine(pointsSq.at(0), pointsSq.at(i));
        } else
              l = sceneTif.painter->meterWidthOfLine(pointsSq.at(i), pointsSq.at(i+1));


        if (l == 0) break;

        uint xNote = side.center().x();  //координаты X лев.верх. угла сноски
        uint yNote = side.center().y();  //координаты Y лев.верх. угла сноски

        //инициал-я стороны участка
        if (i == pointsSq.size()-1)
            side.setPoints(pointsSq.at(0), pointsSq.at(i));
        else
            side.setPoints(pointsSq.at(i), pointsSq.at(i+1));

        int widthNote = QString::number(l).length()*6 + 17; //длина сноски как кол-во цифр * 6 + " м "

        // верхний левый угол сноски
        if (side.center().x() <= boundRect.center().x() && side.center().y() <= boundRect.center().y()) {
          xNote = side.center().x() - widthNote - 1;
          yNote = side.center().y() - heightNote - 1;
        }
        // верхний правый угол сноски
        if (side.center().x() > boundRect.center().x() && side.center().y() <= boundRect.center().y())  {
          xNote = side.center().x() + 1;
          yNote = side.center().y() - heightNote - 1;
        }
        // нижний левый угол сноски
        if (side.center().x() <= boundRect.center().x() && side.center().y() > boundRect.center().y())  {
          xNote = side.center().x() - widthNote - 1;
          yNote = side.center().y() + 1;
        }
        // нижний правый угол сноски
        if (side.center().x() > boundRect.center().x() && side.center().y() > boundRect.center().y())   {
          xNote = side.center().x() + 1;
          yNote = side.center().y() + 1;
        }

        sceneTif.addTextOnMap(QString("%1 м ").arg(l), QFont("Arial", 8, QFont::Bold), QRect(xNote, yNote, widthNote, heightNote), QPoint(xNote, yNote - 3));
        p->addPoint1(QRectF(heightNote-2, widthNote-2, 4, 4), QColor("red"));

        QPen pen;
        pen.setColor(QColor(0, 0, 154, 230));
        pen.setWidth(2);

        //круг на центр стороны участка
        sceneTif.addEllipse(side.center().x()-2, side.center().y()-2, 4, 4,pen, QBrush(QColor(255,255,255,160)));

      //ui->textEdit->append(QString("длина: %1").arg(l));
      }

    }
    else
        showMessage("Не указан участок для измерения сторон");
}

void MainWindow::on_clearMarkup_triggered()
{
    sceneTif.clearMarkupOnPhoto();

}

void MainWindow::on_clearCadastrTree()
{
    qDebug() << "clearCadast";
}

void MainWindow::doScreenShot(QString fName)
{
    if (ui->graphicsViewTif->size().width() > 0) {
        QSize *szPhoto = new QSize(ui->graphicsViewTif->size().width()-10, ui->graphicsViewTif->size().height()-10);
        QPixmap screenShot1 = ui->graphicsViewTif->grab(QRect(QPoint(10, 10), *szPhoto));
        if (screenShot1.size().width() > 0)
            screenShot1.save(fName);

    }
}

void MainWindow::slotErrorProcess(const QString &strErr)
{
    statusBar()->showMessage("Ошибка при создании маски...",10000);
    QMessageBox::warning(this,
                         Settings::Instance().getAppName(),
                         QString("%1").arg(strErr),
                         QMessageBox::Ok);
}

void MainWindow::on_chBox_borders_clicked(bool checked)
{
    sceneTif.setVisibleBorder(checked);
}

void MainWindow::on_chBox_buildings_clicked(bool checked)
{
   sceneTif.setVisibleBuildings(checked);
}

void MainWindow::on_actionOpenFile_triggered()
{
    try
    {   

        QString dirName = QFileDialog::getOpenFileName(this,
             "Открыть файл", "", "Данные  (*.atl)");

        if (dirName.isNull() or dirName.isEmpty())
            return;

        Settings::Instance().setDir(dirName);


        loadPhoto();

    }
    catch (handleMessError &e)
    {
         QMessageBox::warning(0,
                                Settings::Instance().getAppName(),
                                QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                                QMessageBox::Ok);
    }

}

void MainWindow::loadPhoto()
{
    if (!Settings::Instance().dirValid()) return;

    //вывожу заставку
    splashScreen *spl = new splashScreen(this);
    spl->setWindowFlag(Qt::SplashScreen);
    spl->show();
    //QList<QPointF> coord;
    //bool is_coord=false;
    QString fileName=Settings::Instance().getFilePhoto();

    /*try
    {
        parserFileTab *parser= new parserFileTab(fileName);
        coord= parser->getCoordRaster();
        is_coord=true;
        qApp->processEvents();
    }
    catch (handleMessError &e)
    {
    }*/
    try
    {

        /*if(is_coord)
        {
            sceneTif.loadMap(fileName,coord);
            loadXMLFromDir();
            qApp->processEvents();
        }

        else
         sceneTif.loadMap(fileName);*/
        QSizeF size_pix;
        readerParcelsJson *reader=new readerParcelsJson(ui->treeWidget);
        size_pix=reader->readFile();
        sceneTif.loadMap(fileName,size_pix);
        qApp->processEvents();
        drawBorders();
        qApp->processEvents();

        //paintAllCadastr();
        //loadErrors();
        modelViolation->init();
        ui->tv_violation->setColumnHidden(0,true);
        ui->tv_violation->setColumnHidden(4,true);
        qApp->processEvents();

    }
    catch (handleMessError &e)
    {

        QMessageBox::warning(this,
                             this->windowTitle(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
    delete spl;

}



void MainWindow::drawBorders()
{
    QString textSql=QString("select id from border where id_violation in (select id_violation from violation where id_photo=%1)")
            .arg(Settings::Instance().getIdPhoto());
    dbWorker dbWorker_;

    QSqlQuery query=dbWorker_.ExecQuery(textSql);

    while(query.next())
    {
        borderObj *border=new dbBorderObj(&sceneTif,query.value(0).toInt());
        border->draw();
    }
}


void MainWindow::on_actionNext_triggered()
{
    QPointF p=sceneTif.searchNext();
    if(p.x()==-1)
    {
        QMessageBox::warning(this,
                               this->windowTitle(),
                               "Обработка завершена",
                               QMessageBox::Ok);
    }
    else
    {
        qDebug()<<p;
        ui->graphicsViewTif->centerOn(p);
    }

}

void MainWindow::on_actionPrev_triggered()
{
    QPointF p=sceneTif.searchPrev();
    if(p.x()==-1)
    {
        QMessageBox::warning(this,
                               this->windowTitle(),
                               "Обработка завершена",
                               QMessageBox::Ok);
    }
    else
    ui->graphicsViewTif->centerOn(p);
}

void MainWindow::on_actionReport_triggered()
{
    try {

        QModelIndex ind = ui->tv_violation->selectionModel()->currentIndex();
        if (!ind.isValid())
        {
            showMessage("Не выбрано нарушение в списке нарушений ");
            return;
        }
        ReportGenerator::estate_Rec realEstateRec; //данные об объекте недвиж-ти
        int id= ui->tv_violation->model()->index(ind.row(), 0).data().toInt();
        dbWorker dbWorker_;
        QString  textSql=QString("select id,addr,knum from mark where id_violation=%1").arg(id);
        QSqlQuery query=dbWorker_.ExecQuery(textSql);
        int id_mark=0;
        if(query.next())
        {
            id_mark=query.value(0).toInt();
            realEstateRec.adr=query.value("addr").toString();
            realEstateRec.kadNum=query.value("knum").toString();
        }

//        if(id_mark) //ошибка выбора из БД при формир-и отчета участка с кад. № и адресом
//        {
//            textSql=QString("select name,site from firms where id_mark=%1").arg(id_mark);
//            query=dbWorker_.ExecQuery(textSql);
//            QStringList listFirms;
//            while(query.next())
//            {
//                listFirms.append(QString("%1:%2").arg(query.value(0).toString()).arg(query.value(0).toString()));
//            }
//            realEstateRec.info=listFirms.join(", ");
//        }


        int id_type= ui->tv_violation->model()->index(ind.row(), 4).data().toInt();//тип наруш-я

        ReportGenerator *repGen = new ReportGenerator(this->parent() );

        QString folderStr = Settings::Instance().getDir().append("/reports/"); //папка сохр-я отчета
        QString scrShot = QDir::tempPath().append("/scriny"); //путь к cкриншоту а/фотоснимка с нарушением

        QDir dir(folderStr), dirScrShot(scrShot);
        if (!dir.exists(folderStr))
            dir.mkdir(folderStr);

        if (!dirScrShot.exists())
            dirScrShot.mkdir(scrShot);

        if (id_type==2||id_type==3) //незарегистр-й участок или участок вне ИЖС  (reportVneIzhS | repUnregistered)
        {
            scrShot += "/area_vneIzhS.jpg";
            doScreenShot(scrShot);
            repGen->doReportVneIzhS(scrShot, realEstateRec);
            return;
        }

        if (id_type==1 )//выход участка за кадаст. границу (reportZaCadastrTerr)
        {
            scrShot += "/area_zaCadTer.jpg";
            doScreenShot(scrShot);
            repGen->doReportCadastrTerr(scrShot, realEstateRec);
            return;
        }

//        if (!reportVneIzhS && !reportZaCadastrTerr && !repUnregistered) {
//            showMessage("Нарушений не выявлено");
//            return;
//        }

    }
    catch (handleMessError &e)
    {

        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
    }


void MainWindow::on_printInfo_triggered()
{

}

void MainWindow::on_selGranicyButton_clicked()
{
    ui->actionSetPoint->triggered();
}

void MainWindow::on_nextButton_clicked()
{
    ui->actionNext->triggered();
}

/*
void MainWindow::on_allDisorderReportButton_clicked()
{
    QString fotoPath = Settings::Instance().getFilePhoto(), scrinPath = "";
    QPixmap scrin;
    int x1 =0, y1=0;
    QDir dirScr(QDir::tempPath());

    ReportGenerator::estate_Rec realEstateRec; //данные об объекте недвиж-ти
    QMap<QString, ReportGenerator::estate_Rec> resList; //cписок нарушений для формир-я отчета

    if (!dirScr.exists("scriny"))  dirScr.mkdir("scriny");

    FormViolation *form_violation;
    QAbstractItemModel *tv_viol_model = ui->tv_violation->model();

    QProgressDialog *progressDlg = new QProgressDialog("Построение табличного отчета..", "Отмена", 0, tv_viol_model->rowCount());
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setWindowTitle(Settings::Instance().getAppName());
//    progressDlg->setMinimumDuration(1000);
    progressDlg->setAutoClose(true); // показывает прогресс-диалог
    progressDlg->setAutoReset(true); //

    dbWorker _dbWorker;
    QString queryTxt= QString("SELECT name FROM type_violation WHERE id=4 ");
    QSqlQuery sqlQ = _dbWorker.ExecQuery(queryTxt);
    QString typViol = "";
    if (sqlQ.last()) {
        typViol = sqlQ.value(0).toString();
    }

    ReportGenerator *rep = new ReportGenerator();

    //заполн-е нарушениями
    for (int i=0; i< tv_viol_model->rowCount(); i++)
    {
        progressDlg->setValue(i+1);
        qApp->processEvents();
        if (progressDlg->wasCanceled())
            break;

        //пропускаем ложн. срабатывания
//        if (typViol == ui->tv_violation->model()->data(tv_viol_model->index(i, 3)).toString()) {
//            continue;
//        }

        realEstateRec.adr = ui->tv_violation->model()->data(tv_viol_model->index(i, 2)).toString();
        realEstateRec.kadNum = ui->tv_violation->model()->data(tv_viol_model->index(i, 1)).toString();
        form_violation = new FormDBViolation(&sceneTif, noMode, modelViolation->getId(tv_viol_model->index(i, 0)),1);

        QPoint centerP = form_violation->centerMap().toPoint();

        if (ui->graphicsViewTif->size().width() > 0)
        {
          scrinPath = QString("%1/scriny/screen_%2.jpg").arg(QDir::tempPath()).arg(i+1);
          if (centerP.x() >= 200) x1 = centerP.x() - 200;
          if (centerP.y() >= 200) y1 = centerP.y() - 200;
          QSize *szPhoto = new QSize(400,400);
          QPixmap screenShot1 = ui->graphicsViewTif->grab(QRect(QPoint(x1, y1), *szPhoto));
          qDebug()<<"x1"<<"y1"<<x1<<" "<<y1<<scrinPath;
          if (screenShot1.size().width() > 0)  screenShot1.save(scrinPath);
        }
        resList.insert(scrinPath, realEstateRec);
        realEstateRec.adr = "";
        realEstateRec.kadNum = "";
    }
    rep->reportTableWithPhoto( resList);
}

 */

void MainWindow::on_allDisorderReportButton_clicked()
{
    try
    {
        QDir dirScr(QDir::tempPath());
        ReportGenerator::estate_Rec realEstateRec; //данные об объекте недвиж-ти
        QMap<QString, ReportGenerator::estate_Rec> resList; //cписок нарушений для формир-я отчета
        if (!dirScr.exists("scriny"))  dirScr.mkdir("scriny");

        dbWorker dbWorker_;
        QString  textSql="select x,y,id_type,addr,knum,t1.* "
                         " from border left outer join mark on mark.id_violation=border.id_violation,"
                         " (select max(x) as x_max,max(y) as y_max,min(x) as x_min,min(y) as y_min,id_border from point_border group by id_border) as t1"
                         " where t1.id_border=border.id";

        QSqlQuery query=dbWorker_.ExecQuery(textSql);

        int i=0;
        QProgressDialog progressDlg(this);
        progressDlg.setLabelText("Построение табличного отчета..");
        qDebug()<< query.size();
        progressDlg.setRange(0, query.size());
        progressDlg.setModal(true);
        while(query.next())
        {
            i=i+1;
            progressDlg.setValue(i);
            qApp->processEvents();
            if (progressDlg.wasCanceled())
                break;
            if (query.value("id_type").toInt() == 4)
            {
                continue;
            }
            realEstateRec.adr=query.value("addr").toString();
            realEstateRec.kadNum=query.value("knum").toString();

            QPoint p1(query.value("x").toInt(),query.value("y").toInt());
            QPoint p_max(query.value("x_max").toInt(),query.value("y_max").toInt());
            QPoint p_min(query.value("x_min").toInt(),query.value("y_min").toInt());
            if(!p1.isNull())
            {
            if(p1.x()>p_max.x()) p_max.setX(p1.x());
            if(p1.y()>p_max.y()) p_max.setY(p1.y());
            if(p1.x()<p_min.x()) p_min.setX(p1.x());
            if(p1.y()<p_min.y()) p_min.setY(p1.y());
            }
            qDebug()<<p1<<p_min<<p_max;
            QRect rect(p_min,p_max);
            QString scrinPath=createImg(rect,i);
            resList.insert(scrinPath, realEstateRec);
        }
        ReportGenerator *rep = new ReportGenerator();
        rep->reportTableWithPhoto(resList);
    }
    catch (handleMessError &e)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
}

QString MainWindow::createImg(QRect rect,int num)
{

    QString scrinPath = QString("%1/scriny/screen_%2.jpg").arg(QDir::tempPath()).arg(num);
    QFile file(scrinPath);
    if (file.exists()) file.remove();
    ui->graphicsViewTif->centerOn(rect.center());
    QPixmap screenShot1 = ui->graphicsViewTif->grab(QRect(QPoint(0,0),ui->graphicsViewTif->size()));
    screenShot1=screenShot1.scaled(400,400);
    if (screenShot1.size().width() > 0)  screenShot1.save(scrinPath);
    return scrinPath;
}

void MainWindow::on_actionGetNums_triggered()
{
    dbWorker dbWorker_;
    QString  textSql="select knum from mark";
    QSqlQuery query=dbWorker_.ExecQuery(textSql);
    QTextEdit *textEdit;
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        textEdit = qobject_cast<QTextEdit *>(widget);
        if(textEdit) break;
    }
    if (!textEdit)
        textEdit= new QTextEdit();
    bool isEmpty=true;
    textEdit->setWindowTitle("Список кадастровых номеров размеченных участков");
    textEdit->clear();
    while(query.next())
    {
        textEdit->append(query.value(0).toString());
        isEmpty=false;
    }
    if(isEmpty)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             "Нет данных",
                             QMessageBox::Ok);
    }
    else
    {
        textEdit->show();
    }
}

void MainWindow::on_actionUpdateViolation_triggered()
{
    QModelIndex index = ui->tv_violation->selectionModel()->currentIndex();
    showFormViolation(index);

}

void MainWindow::on_actionDeleteViolation_triggered()
{
     QModelIndex index = ui->tv_violation->selectionModel()->currentIndex();
     if(QMessageBox::Yes == QMessageBox::information(0,
                                                   Settings::Instance().getAppName(),
                                                    "Удалить нарушение ?",
                                                    QMessageBox::Yes|QMessageBox::No))
    {
        modelViolation->deleteFromModel(index);

        //удаляю на карте отметки с нанесенной площадью и длинами сторон

    }

}

void MainWindow::on_selUserButton_clicked()
{
    ui->actionAddMark->trigger();
}


void MainWindow::on_allDisorderReport_triggered()
{
    on_allDisorderReportButton_clicked();

}

void MainWindow::on_chBox_cmap_clicked(bool checked)
{
    sceneTif.setVisibleCmap(checked);
}


void MainWindow::loadErrors()
{
    if(!Settings::Instance().dirValid()) return;
    QString fileName=QString("%1%2%3").arg(Settings::Instance().getDir()).arg(QDir::separator()).arg("label_igs.txt");
    fillDataFromFile(fileName,ui->treeWidgetError);
    fileName=QString("%1%2%3").arg(Settings::Instance().getDir()).arg(QDir::separator()).arg("label_cross.txt");
    fillDataFromFile(fileName,ui->treeWidgetCross);

}

void MainWindow::fillDataFromFile(QString fileName,QTreeWidget *treeW)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;


    QTextStream in(&file);
    int num=1;

    while(!in.atEnd())
    {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeW->invisibleRootItem());
        QString line=in.readLine();
        QStringList coord=line.split(' ');

        QString str = QString("Нарушение  %1").arg(num++);
        treeItem->setText(0, str);
        treeItem->setData(0, Qt::UserRole, QVariant(coord));
        //sceneTif.drawErrorRect(QVariant(coord).toList(),Qt::red);
    }
}

void MainWindow::on_treeWidgetError_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QGraphicsRectItem *rect=sceneTif.drawErrorRect(item->data(0,Qt::UserRole).toList());
    ui->graphicsViewTif->centerOn(rect);
}


void MainWindow::on_treeWidgetCross_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QGraphicsRectItem *rect=sceneTif.drawErrorRect(item->data(0,Qt::UserRole).toList());
    ui->graphicsViewTif->centerOn(rect);
}

void MainWindow::readFirms()
{
    QString nameFile=QString("%1/firms.json").arg(Settings::Instance().getDir());
    QFile file(nameFile);
    if(!file.exists())
    {
        QString lastError =QString("Не найден файл %1").arg(nameFile);
        throw  handleMessError(lastError,"Получение данных 2GIS");
    }


    QJsonDocument _json;
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
        _json = QJsonDocument::fromJson(file.readAll());

    if(_json.isArray())
    {
        QJsonArray data=_json.array();
        for(int i=0; i<data.count(); i++)
        {
            QJsonObject data0 = data[i].toObject();
            dbWorker dbWorker_;
            QString  textSql=QString("select id  from mark where x=%1 and y=%2")
                    .arg(data0["x"].toInt()).arg(data0["y"].toInt());
            QSqlQuery query=dbWorker_.ExecQuery(textSql);
            if(!query.next())
            {
                qDebug()<<"Не найден идентификатор в таблице mark";
                continue;
            }
            int id=query.value(0).toInt();
            QJsonObject firms=data0["2gis"].toObject();
            QStringList keys=firms.keys();
            if(keys.count()>0)
            {
                textSql=QString(" delete from firms where id_mark=%1")
                        .arg(id);
                dbWorker_.ExecQuery(textSql);

            }
            for(int i=0; i<keys.count(); i++)
            {
                textSql=QString(" insert into firms(id_mark,name,site) values(%1,'%2','%3')")
                        .arg(id).arg(keys[i]).arg(firms[keys[i]].toString());
                dbWorker_.ExecQuery(textSql);
            }
        }
    }
}


QSizeF MainWindow::readParcels()
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
    /*qDebug()<<pixel_size_m[0]<<pixel_size_m[1];
    QJsonArray parcels = _json["parcels"].toArray();
    QJsonObject obj1=parcels[0].toObject();
    qDebug()<<obj1["points"];*/
    return QSizeF(pixel_size_m[0].toDouble(),pixel_size_m[1].toDouble());

}

void MainWindow::on_action2Gis_triggered()
{
    try
    {
        //if(Settings::Instance().dirValid())
            //throw("Нет загруженных данных","Получение данных 2GIS");

    //createJsonCoord();
    getFirms();
    //QString nameFile="C:/PROJECT/from_git/controlland_light/bin/254-127-Б/firms.json";
    //readFirms(nameFile);
    }
    catch (handleMessError &e)
    {
        QMessageBox::warning(0,
                             Settings::Instance().getAppName(),
                             QString("%1\n\n%2").arg(e.getContext()).arg(e.getMessage()),
                             QMessageBox::Ok);
    }
}

void MainWindow::getFirms()
{
    QString path=Settings::Instance().getDir(); //"C:/PROJECT/from_git/controlland_light/bin/254-127-Б";
    QString pathExe= QString("%1/gis_parser.bat").arg(path);
    if (!QFile(pathExe).exists())
    {
        QString lastError =QString("Не найден файл %1").arg(pathExe);
        throw  handleMessError(lastError,"Получение данных 2GIS");
    }
    QProcess process;
    QString str=QString("%1 \"%2\" ")
            .arg(pathExe).arg(path);
    qDebug()<<str;
    try {

        process.start(str);
        process.waitForFinished();
        if (!process.exitStatus())
        {
            {
                QString lastError =QString("Процесс %1 завершился с ошибкой ").arg(pathExe);
                throw  handleMessError(lastError,"Получение данных 2GIS");
            }
        }
    }

    catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
    /*catch(...)
        {
        qDebug()<<"Ошибка";

        }*/
}


void MainWindow::createJsonCoord()
{
    dbWorker dbWorker_;
    QString  textSql="select id,x,y from mark";
    QSqlQuery query=dbWorker_.ExecQuery(textSql);



   //QString nameFile="C:/PROJECT/from_git/controlland_light/bin/254-127-Б/coords.json";
    QString nameFile=QString("%1/coords.json").arg(Settings::Instance().getDir());
    QFile saveFile(nameFile);

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        QString lastError =QString("Ошибка записи файла %1").arg(nameFile);
        throw  handleMessError(lastError,"Получение данных 2GIS");

    }
    QJsonArray markArray;
    while(query.next())
    {
        QJsonObject coordObject;
        coordObject["point_id"] = query.value(0).toInt();
        coordObject["x"] = query.value(1).toInt();
        coordObject["y"] = query.value(2).toInt();
        markArray.append(coordObject);
    }
    QJsonDocument saveDoc(markArray);
    saveFile.write(saveDoc.toJson());
 }


