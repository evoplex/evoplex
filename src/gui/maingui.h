#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

#include "core/mainapp.h"
#include "gui/wizardnewproject.h"

namespace Ui {
    class MainGUI;
}

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp& mainApp, QWidget* parent=0);
    ~MainGUI();

public slots:
    void addProject(Project* project);

private:
    Ui::MainGUI* m_ui;
    MainApp& m_mainApp;
    WizardNewProject* m_wizardNewProject;

};

#endif // MAINGUI_H
