#include "mainui.h"
#include "dataparser.h"


#include <iostream>
#include <QApplication>
#include <QDebug>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    mainUI mainMenu;
    mainMenu.show();
    return a.exec();
}
