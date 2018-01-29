/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTSPAGE_H
#define PROJECTSPAGE_H

#include <QMainWindow>

#include "maingui.h"
#include "core/mainapp.h"

namespace evoplex {

class ExperimentWidget;
class ProjectWidget;

class ProjectsPage : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProjectsPage(MainGUI* mainGUI);

    inline const ProjectWidget* currentProject() const { return m_currProjectWidget; }

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
    MainGUI* m_mainGUI;
    MainApp* m_mainApp;
    ProjectWidget* m_currProjectWidget;
    QVector<ProjectWidget*> m_projects; // opened projects
    QVector<ExperimentWidget*> m_experiments; // opened experiments

    void addProjectWidget(Project* project);
};
}

#endif // PROJECTSPAGE_H
