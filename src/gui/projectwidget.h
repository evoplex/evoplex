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
#include "tablewidget.h"
#include "core/project.h"

class Ui_ProjectWidget;

namespace evoplex {

class ProjectWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(Project* project, ExperimentsMgr *expMgr, QWidget *parent = 0);
    ~ProjectWidget() {}

signals:
    void openExperiment(int projId, int expId);

private slots:
    void onItemDoubleClicked(QTableWidgetItem* item);
    void insertRow(const int& expId);

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
