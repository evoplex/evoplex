/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTSPAGE_H
#define PROJECTSPAGE_H

#include <QMainWindow>

#include "maingui.h"
#include "core/experiment.h"
#include "core/mainapp.h"

namespace evoplex {

class ExperimentDesigner;
class ExperimentWidget;
class ProjectWidget;

class ProjectsPage : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProjectsPage(MainGUI* mainGUI);
    ~ProjectsPage();

    inline Project* activeProject() const { return m_activeProject; }
    inline const QVector<ProjectWidget*> projects() const { return m_projects; }

signals:
    void activeProjectChanged(Project*);
    void isEmpty(bool empty);
    void hasUnsavedChanges(Project*);

public slots:
    bool slotNewProject();
    bool slotOpenProject(QString path);
    void slotOpenExperiment(Experiment* exp);

private slots:
    void slotFocusChanged(QWidget*, QWidget* now);
    void slotFocusChanged(QDockWidget* dw);

private:
    QSettings m_userPrefs;
    MainGUI* m_mainGUI;
    MainApp* m_mainApp;
    ExperimentDesigner* m_expDesigner;
    Project* m_activeProject;
    QVector<ProjectWidget*> m_projects; // opened projects
    QVector<ExperimentWidget*> m_experiments; // opened experiments

    void addProjectWidget(Project* project);
};

class PPageDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit PPageDockWidget(ProjectsPage* parent) : QDockWidget(parent) {}
    virtual ~PPageDockWidget() {}
    virtual Project* project() const = 0;
    virtual void clearSelection() = 0;
};

}

#endif // PROJECTSPAGE_H
