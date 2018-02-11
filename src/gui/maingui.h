/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QAction>
#include <QMainWindow>

#include "colormap.h"
#include "core/mainapp.h"

namespace evoplex {

class ProjectWidget;
class SaveDialog;

class PluginsPage;
class ProjectsPage;
class QueuePage;
class SettingsPage;
class WelcomePage;

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp* mainApp, QWidget* parent=0);
    ~MainGUI();

    inline MainApp* mainApp() const { return m_mainApp; }
    inline ColorMapMgr* colorMapMgr() const { return m_colorMapMgr; }
    inline SaveDialog* saveDialog() const { return m_saveDialog; }

signals:
    void newProject();
    void openProject(QString path);

private slots:
    void updateSaveButtons(Project* proj);
    void slotPage(QAction* action);
    void slotSave();
    void slotSaveAs();
    void slotSaveAll();
    void slotShowLog();
    void slotShowAbout();

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
        PAGE_SETTINGS
    };

    MainApp* m_mainApp;
    ColorMapMgr* m_colorMapMgr;

    SaveDialog* m_saveDialog;
    WelcomePage* m_welcome;
    QueuePage* m_queue;
    ProjectsPage* m_projectsPage;
    PluginsPage* m_plugins;
    SettingsPage* m_settings;
    Page m_curPage;

    QAction* m_actNewProject;
    QAction* m_actOpenProject;
    QAction* m_actSave;
    QAction* m_actSaveAs;

    void setPageVisible(Page page, bool visible);
};
}
#endif // MAINGUI_H
