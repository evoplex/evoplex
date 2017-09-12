/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QDockWidget>
#include <QMap>
#include <QMainWindow>

#include "core/project.h"
#include "gui/attributeswidget.h"
#include "gui/tablewidget.h"
//#include "gui/contextmenutable.h"

class ProjectWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(Project* project, QWidget *parent = 0);
    ~ProjectWidget();

signals:
    void openExperiment(int projId, int expId);

private slots:
    void onItemDoubleClicked(QTableWidgetItem* item);
    void insertRow(const int& expId);

private:
    QMainWindow* m_innerWindow;
    TableWidget* m_table;
    AttributesWidget* m_attrWidget;
    Project* m_project;

    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    void insertItem(int row, TableWidget::Header header, QString label, QString tooltip="");
};

#endif // PROJECTWIDGET_H
