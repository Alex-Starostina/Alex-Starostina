#include "formstartinfo.h"
#include "ui_formstartinfo.h"
#include "settings.h"

FormStartInfo::FormStartInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormStartInfo)
{
    ui->setupUi(this);
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

    ui->lgdLabel4->setFixedSize(pixmap1.width(), pixmap1.height());
    ui->lgdLabel4->setPixmap(pixmap1);
}

FormStartInfo::~FormStartInfo()
{
    delete ui;
}
