#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>

namespace Ui {
class mainUI;
}

class mainUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainUI(QWidget *parent = nullptr);
    ~mainUI();

private:
    Ui::mainUI *ui;
};

#endif // MAINUI_H
