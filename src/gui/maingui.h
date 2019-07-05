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

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QAction>
#include <QMainWindow>

#include "core/mainapp.h"
#include "colormap.h"

namespace evoplex {

class ConsoleWidget;
class ProjectWidget;
class SaveDialog;

class PluginsPage;
class ProjectsPage;
class GraphDesignerPage;

//class QueuePage;
class SettingsPage;
class WelcomePage;

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    static const QFlags<Qt::WindowType> kDefaultDlgFlags;

    explicit MainGUI(MainApp* mainApp);
    ~MainGUI();

    inline MainApp* mainApp() const { return m_mainApp; }
    inline ColorMapMgr* colorMapMgr() const { return m_colorMapMgr; }
    inline SaveDialog* saveDialog() const { return m_saveDialog; }

signals:
    void newProject();
    void newGraph();
    void openProject(QString path);

private slots:
    void updateSaveButtons(int projId);
    void slotPage(QAction* action);
    void slotSave();
    void slotSaveAs();
    void slotSaveAll();
    void slotShowLog();
    void slotShowAbout();
    void slotCheckedForUpdates(const QVariantMap& data);

protected:
    void closeEvent(QCloseEvent* event);
    bool eventFilter(QObject* o, QEvent* e);

private:
    enum Page {
        PAGE_NULL,
        PAGE_WELCOME,
        PAGE_QUEUE,
        PAGE_PROJECTS,
        PAGE_PLUGINS,
        PAGE_SETTINGS,
        PAGE_GRAPHS
    };

    MainApp* m_mainApp;
    ColorMapMgr* m_colorMapMgr;

    SaveDialog* m_saveDialog;
    WelcomePage* m_welcome;
    //QueuePage* m_queue;
    ProjectsPage* m_projectsPage;
    PluginsPage* m_plugins;
    SettingsPage* m_settings;
    ConsoleWidget* m_console;
    GraphDesignerPage* m_graphPage;
    Page m_curPage;

    QAction* m_actNewProject;
    QAction* m_actOpenProject;
    QAction* m_actNewGraph;
    QAction* m_actSave;
    QAction* m_actSaveAs;

    void setPageVisible(Page page, bool visible);
};
}
#endif // MAINGUI_H
