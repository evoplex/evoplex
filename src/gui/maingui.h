/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

#include "core/mainapp.h"
#include "gui/projectswindow.h"
#include "gui/queuewidget.h"
#include "gui/settingswidget.h"
#include "gui/welcomewidget.h"

class MainGUI: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(MainApp* mainApp, QWidget* parent=0);

public slots:
    void slotPage(QAction* action);
    void newProject() { m_actNewProject->trigger(); }

protected:
    bool eventFilter(QObject *watched, QEvent *event);

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
    WelcomeWidget* m_welcome;
    QueueWidget* m_queue;
    ProjectsWindow* m_projects;
    SettingsWidget* m_settings;
    Page m_curPage;

    QAction* m_actNewProject;

    void setPageVisible(Page page, bool visible);
};

#endif // MAINGUI_H
