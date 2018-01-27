/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QApplication>
#include <QDebug>
#include <QFileDialog>

#include "experimentwidget.h"
#include "projectwidget.h"
#include "projectspage.h"

namespace evoplex {

ProjectsPage::ProjectsPage(MainGUI* mainGUI)
    : QMainWindow(mainGUI)
    , m_mainApp(mainGUI->mainApp())
    , m_currProjectWidget(nullptr)
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));
}

void ProjectsPage::slotFocusChanged(QDockWidget* currTab)
{
    ProjectWidget* pw = qobject_cast<ProjectWidget*>(currTab);
    m_currProjectWidget = pw ? pw : nullptr;
    emit (selectionChanged(pw));
}

void ProjectsPage::addProjectWidget(Project* project)
{
    ProjectWidget* pw = new ProjectWidget(m_mainApp, project, this);
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

    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
    connect(pw, SIGNAL(openExperiment(int,int)), this, SLOT(slotOpenExperiment(int,int)));
    connect(pw, SIGNAL(hasUnsavedChanges(ProjectWidget*)), SIGNAL(hasUnsavedChanges(ProjectWidget*)));
    connect(pw, &ProjectWidget::closed, [this, pw, project]() {
        for (ExperimentWidget* expW : m_experiments) {
            expW->close();
        }
        m_projects.removeOne(pw);
        pw->deleteLater();
        emit (isEmpty(m_projects.isEmpty()));
        m_mainApp->closeProject(project->getId());
    });

    QHash<int, Experiment*>::const_iterator it = project->getExperiments().cbegin();
    for (it; it != project->getExperiments().cend(); ++it) {
        pw->slotInsertRow(it.key());
    }
}

void ProjectsPage::slotNewProject()
{
    addProjectWidget(m_mainApp->newProject());
}

bool ProjectsPage::slotOpenProject()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Project"));
    if (path.isEmpty()) {
        return false;
    }
    Project* project = m_mainApp->openProject(path);
    if (!project) {
        return false;
    }
    addProjectWidget(project);
    return true;
}

void ProjectsPage::slotOpenExperiment(int projId, int expId)
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
        connect(ew, &ExperimentWidget::closed, [this, ew]() {
            m_projects.last()->raise();
            m_experiments.removeOne(ew);
            ew->deleteLater();
        });

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
