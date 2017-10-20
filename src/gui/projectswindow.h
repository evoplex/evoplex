/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTSWINDOW_H
#define PROJECTSWINDOW_H

#include <QMainWindow>

#include "core/mainapp.h"

namespace evoplex {

class ProjectWidget;

class ProjectsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProjectsWindow(MainApp* mainApp, QWidget *parent = 0);

    inline const ProjectWidget* currentProject() const { return m_currentProject; }

signals:
    void selectionChanged(ProjectWidget*);
    void isEmpty(bool empty);

public slots:
    void slotNewProject();
    void slotOpenExperiment(int projId, int expId);

private slots:
    void slotFocusChanged(QDockWidget* currTab);

private:
    MainApp* m_mainApp;
    ProjectWidget* m_currentProject;
    QVector<ProjectWidget*> m_projects; // opened projects
};
}

#endif // PROJECTSWINDOW_H
