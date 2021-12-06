#include "mainui.h"

#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bitcoinAnalyzer::mainUI mainMenu;
    mainMenu.show();
    return a.exec();
}
