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

#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QCloseEvent>
#include <QDockWidget>
#include <QMap>
#include <QMainWindow>

#include "experimentdesigner.h"
#include "projectspage.h"
#include "tablewidget.h"
#include "core/project.h"

class Ui_ProjectWidget;

namespace evoplex {

class ProjectWidget : public PPageDockWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(ProjectPtr project, MainGUI* mainGUI, ProjectsPage* ppage);
    ~ProjectWidget() override;

    inline ProjectPtr project() const override { return m_project; }

    void clearSelection() override;

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* event) override;

signals:
    void expSelectionChanged(int expId);
    void openExperiment(int expId);
    void hasUnsavedChanges(int projId);

public slots:
    void slotInsertRow(int expId);
    void slotUpdateRow(int expId);
    void slotRemoveRow(int expId);
    void slotHasUnsavedChanges(bool b);

private slots:
    void slotSelectionChanged();
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    Ui_ProjectWidget* m_ui;
    MainGUI* m_mainGUI;
    ProjectPtr m_project;

    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    void fillRow(int row, const ExperimentPtr& exp);

    void insertItem(int row, TableWidget::Header header, QString label, QString tooltip="");
};
}
#endif // PROJECTWIDGET_H
