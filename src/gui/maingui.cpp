#include "ui_maingui.h"
#include "gui/maingui.h"

MainGUI::MainGUI(MainApp& mainApp, QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainGUI)
    , m_mainApp(mainApp)
{
    m_ui->setupUi(this);
}

MainGUI::~MainGUI()
{
    delete m_ui;
}
