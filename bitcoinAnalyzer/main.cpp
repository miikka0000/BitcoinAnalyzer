#include "mainui.h"

#include <iostream>
#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainUI mainMenu;
    mainMenu.show();
    return a.exec();
}
