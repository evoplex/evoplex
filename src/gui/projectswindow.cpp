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
    , m_currentProject(nullptr)
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));
}

void ProjectsWindow::slotFocusChanged(QDockWidget* currTab)
{
    ProjectWidget* pw = qobject_cast<ProjectWidget*>(currTab);
    m_currentProject = pw ? pw : nullptr;
    emit (selectionChanged(pw));
}

void ProjectsWindow::slotNewProject()
{
    ProjectWidget* pw = new ProjectWidget(m_mainApp->getProject(m_mainApp->newProject()),
                                          m_mainApp->getExperimentsMgr(), this);
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
    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
}

void ProjectsWindow::slotOpenExperiment(int projId, int expId)
{
    ExperimentWidget* ew = new ExperimentWidget(m_mainApp->getProject(projId), expId, this);
    if (m_projects.isEmpty()) {
        this->addDockWidget(Qt::TopDockWidgetArea, ew);
    } else {
        this->tabifyDockWidget(m_projects.last(), ew);
    }
    ew->show();
    ew->raise();
}

}
