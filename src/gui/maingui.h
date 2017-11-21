/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINGUI_H
#define MAINGUI_H

#include <QAction>
#include <QMainWindow>

#include "projectswindow.h"
#include "queuewidget.h"
#include "savedialog.h"
#include "settingswidget.h"
#include "welcomewidget.h"
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
    WelcomeWidget* m_welcome;
    QueueWidget* m_queue;
    ProjectsWindow* m_projects;
    SettingsWidget* m_settings;
    Page m_curPage;

    QAction* m_actNewProject;
    QAction* m_actOpenProject;
    QAction* m_actSave;
    QAction* m_actSaveAs;

    void setPageVisible(Page page, bool visible);
};
}
#endif // MAINGUI_H
