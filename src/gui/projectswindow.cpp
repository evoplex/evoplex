/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QApplication>
#include <QDebug>

#include "experimentwidget.h"
#include "projectwidget.h"
#include "projectswindow.h"

namespace evoplex {

ProjectsWindow::ProjectsWindow(MainApp* mainApp, QWidget *parent)
    : QMainWindow(parent)
    , m_mainApp(mainApp)
    , m_currProjectWidget(nullptr)
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));
}

void ProjectsWindow::slotFocusChanged(QDockWidget* currTab)
{
    ProjectWidget* pw = qobject_cast<ProjectWidget*>(currTab);
    m_currProjectWidget = pw ? pw : nullptr;
    emit (selectionChanged(pw));
}

void ProjectsWindow::slotNewProject()
{
    Project* project = m_mainApp->newProject();
    ProjectWidget* pw = new ProjectWidget(project, this);
    if (m_projects.isEmpty()) {
        this->addDockWidget(Qt::TopDockWidgetArea, pw);
    } else {
        this->tabifyDockWidget(m_projects.last(), pw);
    }
    pw->show();
    pw->raise();
    m_projects.push_back(pw);
    slotFocusChanged(pw);
    emit (isEmpty(false));
    connect(pw, SIGNAL(openExperiment(int,int)), this, SLOT(slotOpenExperiment(int,int)));
    connect(pw, SIGNAL(hasUnsavedChanges(ProjectWidget*)), SIGNAL(hasUnsavedChanges(ProjectWidget*)));
    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
}

void ProjectsWindow::slotOpenExperiment(int projId, int expId)
{
    ExperimentWidget* ew = nullptr;
    foreach (ExperimentWidget* e, m_experiments) {
        if (e->expId() == expId && e->projId() == projId) {
            ew = e;
            break;
        }
    }

    if (!ew) {
        ew = new ExperimentWidget(m_mainApp->getProject(projId)->getExperiment(expId), this);
        connect(ew, &ExperimentWidget::closed,
                [this, ew](){ m_experiments.removeOne(ew); ew->deleteLater(); });

        if (m_projects.isEmpty() && m_experiments.isEmpty()) {
            addDockWidget(Qt::TopDockWidgetArea, ew);
        } else {
            tabifyDockWidget(m_projects.last(), ew);
        }
        m_experiments.append(ew);
    }
    ew->show();
    ew->raise();
}
}
