#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(QWidget *parent = 0);
    ~MainGUI();

};

#endif // MAINGUI_H
