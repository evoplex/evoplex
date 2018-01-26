/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QAction>
#include <QMainWindow>

#include "pluginspage.h"
#include "projectspage.h"
#include "queuepage.h"
#include "savedialog.h"
#include "settingspage.h"
#include "welcomepage.h"
#include "core/mainapp.h"

namespace evoplex {

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp* mainApp, QWidget* parent=0);

signals:
    void newProject();
    void openProject();

private slots:
    void updateSaveButtons(ProjectWidget* pw);
    void slotPage(QAction* action);
    void slotSave();
    void slotSaveAs();
    void slotSaveAll();

protected:
    void closeEvent(QCloseEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);

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
    SaveDialog* m_saveDialog;
    WelcomePage* m_welcome;
    QueuePage* m_queue;
    ProjectsPage* m_projects;
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
