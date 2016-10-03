#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

namespace Ui {
    class MainGUI;
}

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(QWidget* parent = 0);
    ~MainGUI();

private:
    Ui::MainGUI* ui;

};

#endif // MAINGUI_H
