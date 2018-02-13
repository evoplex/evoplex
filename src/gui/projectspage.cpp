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
    , m_expDesigner(new ExperimentDesigner(mainGUI->mainApp(), this))
    , m_activeProject(nullptr)
{
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);
    setDockNestingEnabled(true);
    setAnimated(true);
    setCentralWidget(0);

    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            SLOT(slotFocusChanged(QWidget*,QWidget*)));

    addDockWidget(Qt::RightDockWidgetArea, m_expDesigner);

    restoreGeometry(m_userPrefs.value("gui/projectsPageGeometry").toByteArray());
    restoreState(m_userPrefs.value("gui/projectsPageState").toByteArray());
}

ProjectsPage::~ProjectsPage()
{
    m_userPrefs.setValue("gui/projectsPageGeometry", saveGeometry());
    m_userPrefs.setValue("gui/projectsPageState", saveState());
}

void ProjectsPage::slotFocusChanged(QDockWidget* dw)
{
    PPageDockWidget* pdw = qobject_cast<PPageDockWidget*>(dw);
    if (pdw) {
        pdw->clearSelection();
        m_activeProject = pdw->project();
        m_expDesigner->setActiveWidget(pdw);
        emit (activeProjectChanged(m_activeProject));
    }
}

void ProjectsPage::slotFocusChanged(QWidget*, QWidget* now)
{
    if (!isVisible()) {
        return;
    }

    QWidget* widget = now;
    PPageDockWidget* dockWidget = nullptr;
    while (widget && widget != this && !dockWidget) {
        dockWidget = qobject_cast<PPageDockWidget*>(widget);
        if (dockWidget) {
            slotFocusChanged(qobject_cast<QDockWidget*>(dockWidget));
        }
        widget = widget->parentWidget();
    }
}

void ProjectsPage::addProjectWidget(Project* project)
{
    ProjectWidget* pw = new ProjectWidget(project, m_mainGUI, this);
    if (m_projects.isEmpty()) {
        addDockWidget(Qt::LeftDockWidgetArea, pw);
    } else {
        tabifyDockWidget(m_projects.last(), pw);
    }
    m_expDesigner->addWidgetToList(pw);
    pw->show();
    pw->raise();
    m_projects.push_back(pw);
    slotFocusChanged(pw);
    emit (isEmpty(false));

    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
    connect(pw, &ProjectWidget::expSelectionChanged, [this](Experiment* exp) { m_expDesigner->setExperiment(exp); });
    connect(pw, SIGNAL(openExperiment(Experiment*)), this, SLOT(slotOpenExperiment(Experiment*)));
    connect(pw, SIGNAL(hasUnsavedChanges(Project*)), SIGNAL(hasUnsavedChanges(Project*)));
    connect(pw, &ProjectWidget::closed, [this, pw, project]() {
        for (ExperimentWidget* expW : m_experiments) {
            if (expW->exp()->project() == project) {
                expW->close();
            }
        }
        m_expDesigner->removeWidgetFromList(pw);
        m_projects.removeOne(pw);
        emit (isEmpty(m_projects.isEmpty()));
        m_mainApp->closeProject(project->id());
        pw->deleteLater();
    });

    for (auto& i : project->experiments()) {
        pw->slotInsertRow(i.second);
    }
}

bool ProjectsPage::slotNewProject()
{
    QString error;
    Project* p = m_mainApp->newProject(error);
    if (p) {
        addProjectWidget(p);
    } else {
        QMessageBox::warning(this, "Evoplex", error);
        return false;
    }
    return true;
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
    Project* project = m_mainApp->newProject(error, path);
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
        if (exp == e->exp()) {
            ew = e;
            break;
        }
    }

    if (!ew) {
        ew = new ExperimentWidget(exp, m_mainGUI, this);
        connect(ew, &ExperimentWidget::closed, [this, ew]() {
            m_projects.last()->raise();
            m_expDesigner->removeWidgetFromList(ew);
            m_experiments.removeOne(ew);
            ew->deleteLater();
        });

        if (m_projects.isEmpty() && m_experiments.isEmpty()) {
            addDockWidget(Qt::LeftDockWidgetArea, ew);
        } else {
            tabifyDockWidget(m_projects.last(), ew);
        }
        m_experiments.append(ew);
    }

    m_expDesigner->addWidgetToList(ew);
    ew->show();
    ew->raise();
}
}
