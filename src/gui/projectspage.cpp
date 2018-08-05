/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    setCentralWidget(nullptr);

    connect(this, SIGNAL(tabifiedDockWidgetActivated(QDockWidget*)),
            SLOT(slotFocusChanged(QDockWidget*)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            SLOT(slotFocusChanged(QWidget*,QWidget*)));

    restoreGeometry(m_userPrefs.value("gui/projectsPageGeometry").toByteArray());
    restoreState(m_userPrefs.value("gui/projectsPageState").toByteArray());

    addDockWidget(Qt::RightDockWidgetArea, m_expDesigner);
}

ProjectsPage::~ProjectsPage()
{
    m_userPrefs.setValue("gui/projectsPageGeometry", saveGeometry());
    m_userPrefs.setValue("gui/projectsPageState", saveState());
    m_expWidgets.clear();
    m_projWidgets.clear();
}

void ProjectsPage::slotFocusChanged(QDockWidget* dw)
{
    PPageDockWidget* pdw = qobject_cast<PPageDockWidget*>(dw);
    if (pdw) {
        pdw->clearSelection();
        m_activeProject = pdw->project();
        m_expDesigner->setActiveWidget(pdw);
        emit (activeProjectChanged(m_activeProject->id()));
    } else {
        m_activeProject = nullptr;
        emit (activeProjectChanged(-1));
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

void ProjectsPage::addProjectWidget(ProjectPtr project)
{
    ProjectWidget* pw = new ProjectWidget(project, m_mainGUI, this);
    if (m_projWidgets.isEmpty()) {
        addDockWidget(Qt::LeftDockWidgetArea, pw);
    } else {
        tabifyDockWidget(m_projWidgets.last(), pw);
    }
    m_expDesigner->addWidgetToList(pw);
    pw->show();
    pw->raise();
    m_projWidgets.push_back(pw);
    slotFocusChanged(pw);
    emit (isEmpty(false));

    ProjectWPtr wProj = project;

    //connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
    connect(pw, SIGNAL(hasUnsavedChanges(int)), SIGNAL(hasUnsavedChanges(int)));

    connect(pw, &ProjectWidget::openExperiment, [this, wProj](int expId) {
        ProjectPtr proj = wProj.lock();
        if (proj) slotOpenExperiment(proj->experiment(expId));
    });

    connect(pw, &ProjectWidget::expSelectionChanged, [this, wProj](int expId) {
        ProjectPtr proj = wProj.lock();
        if (proj) m_expDesigner->setExperiment(proj->experiment(expId));
    });

    connect(pw, &ProjectWidget::closed, [this, pw]() {
        ProjectPtr project = pw->project();
        for (ExperimentWidget* ew : m_expWidgets) {
            if (ew->project() == project) {
                ew->close();
            }
        }
        m_expDesigner->removeWidgetFromList(pw);
        m_projWidgets.removeOne(pw);
        emit (isEmpty(m_projWidgets.isEmpty()));
        m_mainApp->closeProject(project->id());
        pw->deleteLater();
        if (m_projWidgets.isEmpty()) {
            m_activeProject = nullptr;
            emit (activeProjectChanged(-1));
        }
    });

    for (auto& i : project->experiments()) {
        pw->slotInsertRow(i.second->id());
    }
}

bool ProjectsPage::slotNewProject()
{
    QString error;
    ProjectPtr p = m_mainApp->newProject(error);
    if (!p) {
        QMessageBox::warning(this, "Evoplex", error);
        return false;
    }
    addProjectWidget(p);
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
    ProjectPtr project = m_mainApp->newProject(error, path);
    if (!project) {
        QMessageBox::critical(this, "Evoplex", error);
        return false;
    } else if (!error.isEmpty()) {
        QString msg = "A few experiments could not be loaded correctly.\n"
                      "Would you like to open it anyway?\n"
                      "Note: experiments with critical errors won't be opened anyway.";
        QMessageBox box(QMessageBox::Warning, "Opening project...", msg,
                        QMessageBox::No | QMessageBox::Yes, this);
        box.setDetailedText(error);
        if (box.exec() == QMessageBox::No) {
            return false;
        }
    }
    addProjectWidget(project);
    return true;
}

void ProjectsPage::slotOpenExperiment(ExperimentPtr exp)
{
    if (!exp || exp->expStatus() == Status::Invalid) {
        QMessageBox::warning(this, "Experiment",
                "This experiment is invalid.\n"
                "It seems that something went wrong with its settings.\n"
                "Please, use the Experiment Designer tool to check your settings and try again.");
        return;
    }

    auto findEW = [this](ExperimentPtr exp) {
        ExperimentWidget* ret = nullptr;
        for (ExperimentWidget* ew : m_expWidgets) {
            if (ew->exp() == exp) { ret = ew; break; }
        }
        return ret;
    };

    ExperimentWidget* ew = findEW(exp);
    if (!ew) {
        ew = new ExperimentWidget(exp, m_mainGUI, this);
        std::weak_ptr<Experiment> wExp = exp;
        connect(exp->project().get(), &Project::expRemoved, [findEW, wExp](int expId) {
            ExperimentPtr exp = wExp.lock();
            if (exp && expId == exp->id()) {
                auto ew = findEW(exp);
                if (ew) { ew->close(); }
            }
        });
        connect(ew, &ExperimentWidget::closed, [this, ew]() {
            m_projWidgets.last()->raise();
            m_expDesigner->removeWidgetFromList(ew);
            m_expWidgets.removeOne(ew);
            ew->deleteLater();
        });

        if (m_projWidgets.isEmpty() && m_expWidgets.empty()) {
            addDockWidget(Qt::LeftDockWidgetArea, ew);
        } else {
            tabifyDockWidget(m_projWidgets.last(), ew);
        }
        m_expWidgets.push_back(ew);
    }

    m_expDesigner->addWidgetToList(ew);
    ew->show();
    ew->raise();
}
}
