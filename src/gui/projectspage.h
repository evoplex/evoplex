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

#ifndef PROJECTSPAGE_H
#define PROJECTSPAGE_H

#include <QDockWidget>
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

    inline ProjectPtr activeProject() const { return m_activeProject; }
    inline const QVector<ProjectWidget*> projects() const { return m_projects; }

signals:
    void activeProjectChanged(ProjectPtr);
    void isEmpty(bool empty);
    void hasUnsavedChanges(ProjectPtr);

public slots:
    bool slotNewProject();
    bool slotOpenProject(QString path);
    void slotOpenExperiment(ExperimentPtr exp);

private slots:
    void slotFocusChanged(QWidget*, QWidget* now);
    void slotFocusChanged(QDockWidget* dw);

private:
    QSettings m_userPrefs;
    MainGUI* m_mainGUI;
    MainApp* m_mainApp;
    ExperimentDesigner* m_expDesigner;
    ProjectPtr m_activeProject;
    QVector<ProjectWidget*> m_projects; // opened projects
    QVector<ExperimentWidget*> m_experiments; // opened experiments

    void addProjectWidget(ProjectPtr project);
};

class PPageDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit PPageDockWidget(ProjectsPage* parent) : QDockWidget(parent) {}
    virtual ~PPageDockWidget() {}
    virtual ProjectPtr project() const = 0;
    virtual void clearSelection() = 0;
};

} // evoplex
#endif // PROJECTSPAGE_H
