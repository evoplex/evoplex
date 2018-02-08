/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "experimentwidget.h"
#include "projectwidget.h"
#include "projectspage.h"

namespace evoplex {

ProjectsPage::ProjectsPage(MainGUI* mainGUI)
    : QMainWindow(mainGUI)
    , m_mainGUI(mainGUI)
    , m_mainApp(mainGUI->mainApp())
    , m_attrWidget(new AttributesWidget(mainGUI->mainApp(), this))
    , m_activeProject(nullptr)
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));

    addDockWidget(Qt::RightDockWidgetArea, m_attrWidget);
}

void ProjectsPage::showEvent(QShowEvent* e)
{
    QSettings s;
    restoreState(s.value("gui/projectsPage").toByteArray());
    QWidget::showEvent(e);
}

void ProjectsPage::hideEvent(QHideEvent* e)
{
    if (!m_projects.isEmpty()) { // otherwise we'd always save the attrsWidget in fullscreen
        QSettings s;
        s.setValue("gui/projectsPage", saveState());
    }
    QMainWindow::hideEvent(e);
}

void ProjectsPage::slotFocusChanged(QDockWidget* currTab)
{
    m_activeProject = nullptr;
    ProjectWidget* pw = qobject_cast<ProjectWidget*>(currTab);
    if (pw) {
        pw->clearSelection();
        m_activeProject = pw->project();
    } else {
        ExperimentWidget* ew = qobject_cast<ExperimentWidget*>(currTab);
        m_activeProject = ew ? ew->experiment()->project() : nullptr;
    }

    if (m_activeProject) {
        m_attrWidget->setActiveWidget(currTab, m_activeProject);
        emit (activeProjectChanged(m_activeProject));
    }
}

void ProjectsPage::addProjectWidget(Project* project)
{
    ProjectWidget* pw = new ProjectWidget(m_mainGUI, project, this);
    if (m_projects.isEmpty()) {
        addDockWidget(Qt::LeftDockWidgetArea, pw);
    } else {
        tabifyDockWidget(m_projects.last(), pw);
    }
    m_attrWidget->addWidgetToList(pw);
    pw->show();
    pw->raise();
    m_projects.push_back(pw);
    slotFocusChanged(pw);
    emit (isEmpty(false));

    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
    connect(pw, &ProjectWidget::expSelectionChanged, [this](Experiment* exp) { m_attrWidget->setExperiment(exp); });
    connect(pw, SIGNAL(openExperiment(Experiment*)), this, SLOT(slotOpenExperiment(Experiment*)));
    connect(pw, SIGNAL(hasUnsavedChanges(Project*)), SIGNAL(hasUnsavedChanges(Project*)));
    connect(pw, &ProjectWidget::closed, [this, pw, project]() {
        m_attrWidget->removeWidgetFromList(pw);
        for (ExperimentWidget* expW : m_experiments) {
            expW->close();
        }
        m_projects.removeOne(pw);
        if (m_projects.isEmpty()) {
            QSettings s;
            s.setValue("gui/projectsPage", saveState());
            emit (isEmpty(true));
        } else {
            emit (isEmpty(false));
        }
        m_mainApp->closeProject(project->id());
        pw->deleteLater();
    });

    QHash<int, Experiment*>::const_iterator it = project->experiments().cbegin();
    for (it; it != project->experiments().cend(); ++it) {
        pw->slotInsertRow(it.key());
    }
}

void ProjectsPage::slotNewProject()
{
    addProjectWidget(m_mainApp->newProject());
}

bool ProjectsPage::slotOpenProject(QString path)
{
    if (path.isEmpty()) {
        path = QFileDialog::getOpenFileName(this, tr("Open Project"));
        if (path.isEmpty()) {
            return false;
        }
    }

    QString error;
    Project* project = m_mainApp->openProject(path, error);
    if (!project) {
        QMessageBox::warning(this, "Evoplex", error);
        return false;
    }
    addProjectWidget(project);
    return true;
}

void ProjectsPage::slotOpenExperiment(Experiment* exp)
{
    if (!exp) {
        return;
    }

    ExperimentWidget* ew = nullptr;
    foreach (ExperimentWidget* e, m_experiments) {
        if (exp == e->experiment()) {
            ew = e;
            break;
        }
    }

    if (!ew) {
        ew = new ExperimentWidget(m_mainGUI, exp, this);
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

    m_attrWidget->addWidgetToList(ew);
    ew->show();
    ew->raise();
}
}
