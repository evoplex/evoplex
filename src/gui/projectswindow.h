/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTSWINDOW_H
#define PROJECTSWINDOW_H

#include <QMainWindow>

#include "gui/projectwidget.h"

class ProjectsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProjectsWindow(MainApp* mainApp, QWidget *parent = 0);

signals:
    void isEmpty(bool empty);

public slots:
    void slotNewProject();
    void slotOpenExperiment(int projId, int expId);

private:
    MainApp* m_mainApp;

    QVector<ProjectWidget*> m_projects; // opened projects
};

#endif // PROJECTSWINDOW_H
