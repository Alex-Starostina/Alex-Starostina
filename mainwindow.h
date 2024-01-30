#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mapimagescene.h"
#include <QTreeWidgetItem>
#include <QAxObject>
#include <QException>
#include "listpolygons.h"
#include "formviolation.h"
#include "violationsqlmodel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();




    void showMessage(QString msg);

    FormViolation *form_violation;
    void loadPhoto();

public slots:
//    void calculateSquare();
    void centerOnView(QListWidgetItem *);
    void deleteFormViolation(bool refresh);


private:
    Ui::MainWindow *ui;
    QString pathPhoto;

    MapImageScene sceneTif;
    QPoint glPosContextMenu;

    int numPlot;
    QMenu *popupMenu;

    //listPolygons *listErrors;
    //listPolygons *listAreas;
    QMenu *popupMenu_Violation;

    int sel_tv_violation_index;
    bool reportVneIzhS = false; //печатаем отчет для здания вне зоны ИЖС
    bool reportZaCadastrTerr = false; //отчет для выхода здания за кадастр. границу
    bool repUnregistered = false; //отчет для незарегистр. участка

    void addToTree(QList<QVariant > listOfPoints,int num, int idItem);


    void doScreenShot(QString);

    void init_menu();


    void writeSettings();

    void readSettings();
    void loadXML(QString dir);
    void loadXMLFromDir();
    void addViolation(TModeScene mode_);
    void fillStartInfo();
    void loadErrors();
    void fillDataFromFile(QString fileName,QTreeWidget *treeW);
    QString createImg(QRect rect,int num);


private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item );
    void on_btnPaintAll_clicked();
    void on_btnClear_clicked();
    void on_btnLoad_clicked();
    void tableContextMenuRequested(const QPoint &pos);
    void on_actionAddMark_triggered();
    void on_actionSetPoint_triggered();
//    void on_printInfo_triggered();
    void on_calculateSquare_triggered();
    void on_meterWidth_triggered();

    void showContextMenu(const QPoint&);
    void showContexMenu_violation(const QPoint &pos);
    void tv_violation_clicked(const QModelIndex&);


    void on_clearMarkup_triggered();
    void on_clearCadastrTree();

   void slotErrorProcess(const QString &strErr);
    void on_chBox_borders_clicked(bool checked);
    void on_chBox_buildings_clicked(bool checked);


    void on_actionSelecterror_triggered();
    void on_actionOpenFile_triggered();


    void on_actionNext_triggered();

    void on_actionPrev_triggered();

    void on_actionReport_triggered();
    void paintAllCadastr();

    void on_printInfo_triggered();

    void on_selGranicyButton_clicked();

    void on_nextButton_clicked();
    void  showFormViolation(const QModelIndex &);
    void on_allDisorderReportButton_clicked();


    void on_actionGetNums_triggered();

    void on_actionUpdateViolation_triggered();

    void on_actionDeleteViolation_triggered();

    void on_selUserButton_clicked();


    void on_allDisorderReport_triggered();



    void on_chBox_cmap_clicked(bool checked);
void deleteBorder(int id);
void on_treeWidgetError_itemClicked(QTreeWidgetItem *item, int column);

void on_treeWidgetCross_itemClicked(QTreeWidgetItem *item, int column);



void on_action2Gis_triggered();


protected:
    ViolationSqlModel *modelViolation;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void drawBorders();
    void readFirms();
    void createJsonCoord();
    QSizeF readParcels();
    void getFirms();



};


#endif // MAINWINDOW_H
