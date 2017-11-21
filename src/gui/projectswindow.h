/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTSWINDOW_H
#define PROJECTSWINDOW_H

#include <QMainWindow>

#include "core/mainapp.h"

namespace evoplex {

class ExperimentWidget;
class ProjectWidget;

class ProjectsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProjectsWindow(MainApp* mainApp, QWidget *parent = 0);

    inline const ProjectWidget* currentProject() const { return m_currProjectWidget; }
    inline MainApp* getMainApp() const { return m_mainApp; }

signals:
    void selectionChanged(ProjectWidget*);
    void isEmpty(bool empty);
    void hasUnsavedChanges(ProjectWidget*);

public slots:
    void slotNewProject();
    bool slotOpenProject();
    void slotOpenExperiment(int projId, int expId);

private slots:
    void slotFocusChanged(QDockWidget* currTab);

private:
    MainApp* m_mainApp;
    ProjectWidget* m_currProjectWidget;
    QVector<ProjectWidget*> m_projects; // opened projects
    QVector<ExperimentWidget*> m_experiments; // opened experiments

    void addProjectWidget(Project* project);
};
}

#endif // PROJECTSWINDOW_H
