#include "splashscreen.h"
#include "ui_splashscreen.h"
#include <QMovie>

splashScreen::splashScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::splashScreen)
{
    ui->setupUi(this);

}

splashScreen::~splashScreen()
{
    delete ui;
}

