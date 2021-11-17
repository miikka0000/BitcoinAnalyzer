#include "mainui.h"
#include "ui_mainui.h"

mainUI::mainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainUI)
{
    ui->setupUi(this);
}

mainUI::~mainUI()
{
    delete ui;
}
