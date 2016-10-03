#include "maingui.h"
#include "ui_maingui.h"

#include <QtDebug>

MainGUI::MainGUI(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
{
    ui->setupUi(this);
}

MainGUI::~MainGUI()
{
    delete ui;
}
