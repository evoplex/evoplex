#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

#include "core/mainapp.h"

namespace Ui {
    class MainGUI;
}

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp& mainApp, QWidget* parent=0);
    ~MainGUI();

private:
    Ui::MainGUI* m_ui;
    MainApp& m_mainApp;

};

#endif // MAINGUI_H
