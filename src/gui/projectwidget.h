/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QDockWidget>
#include <QMap>
#include <QMainWindow>

#include "attributeswidget.h"
#include "projectswindow.h"
#include "tablewidget.h"
#include "core/project.h"

class Ui_ProjectWidget;

namespace evoplex {

class ProjectWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(Project* project, ProjectsWindow* pwindow = 0);
    ~ProjectWidget() {}

    inline Project* getProject() const { return m_project; }

signals:
    void openExperiment(int projId, int expId);
    void hasUnsavedChanges(ProjectWidget*);

public slots:
    void slotInsertRow(int expId);
    void slotHasUnsavedChanges(bool b);

private slots:
    void onItemClicked(QTableWidgetItem* item);
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    Ui_ProjectWidget* m_ui;
    QMainWindow* m_innerWindow;
    AttributesWidget* m_attrWidget;
    Project* m_project;

    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    void insertItem(int row, TableWidget::Header header, QString label, QString tooltip="");
};
}
#endif // PROJECTWIDGET_H
