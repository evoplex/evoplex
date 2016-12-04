/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

#include "core/mainapp.h"
#include "gui/contextmenutable.h"
#include "gui/wizardnewproject.h"

#define STRING_EXPERIMENT_ID "expId"
#define STRING_PROCESS_ID "procId"
#define STRING_PROCESS_STATUS "status"
#define STRING_MODEL_NAME "model"
#define STRING_PROJECT_NAME "project"

namespace Ui {
    class MainGUI;
}

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp* mainApp, QWidget* parent=0);
    ~MainGUI();

    void addProject(int projId);

    const QString statusToString(Simulation::Status status) const;

public slots:
    void slotAddProcess(int processId);
    void slotStatusChanged(int experimentId, int processId, int newStatus);
    void slotContextMenu(QPoint point);

private:
    Ui::MainGUI* m_ui;
    MainApp* m_mainApp;
    WizardNewProject* m_wizardNewProject;

    QHash<QString, int> m_tableHeader; // map column name to column index
    ContextMenuTable* m_contextMenu;
};

#endif // MAINGUI_H
