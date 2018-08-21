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

#ifndef EXPERIMENT_DESIGNER_H
#define EXPERIMENT_DESIGNER_H

#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QHash>

#include "core/project.h"

#include "external/qt-material-widgets/qtmaterialiconbutton.h"

#include "attrwidget.h"
#include "projectspage.h"
#include "titlebar.h"

class Ui_ExperimentDesigner;

namespace evoplex {

class ExperimentDesigner : public QDockWidget
{
    Q_OBJECT

public:
    explicit ExperimentDesigner(MainApp* mainApp, QWidget* parent);
    ~ExperimentDesigner();

    void addWidgetToList(PPageDockWidget* dw);
    void removeWidgetFromList(PPageDockWidget* dw);
    void setActiveWidget(PPageDockWidget* dw);
    PPageDockWidget* activeWidget() const;

    void setExperiment(ExperimentPtr exp);

private slots:
    void slotEdgesWidget();
    void slotNodesWidget();
    void slotOutputWidget();
    void slotCreateExperiment();
    void slotRemoveExperiment();
    void slotEditExperiment();
    void slotModelSelected(int cbIdx);
    void slotGraphSelected(int cbIdx);
    void slotPluginAdded(const Plugin* plugin);
    void slotPluginRemoved(PluginKey key, PluginType type);
    void slotSetActiveWidget(int idx);

private:
    MainApp* m_mainApp;
    ProjectPtr m_project;
    std::weak_ptr<Experiment> m_exp;
    PluginKey m_selectedGraphKey;
    PluginKey m_selectedModelKey;

    TitleBar* m_titleBar;
    Ui_ExperimentDesigner* m_ui;
    QtMaterialIconButton* m_bRemove;
    QtMaterialIconButton* m_bEdit;
    QtMaterialIconButton* m_bAdd;
    QTreeWidgetItem* m_treeItemGeneral;
    QTreeWidgetItem* m_treeItemOutputs;
    QTreeWidgetItem* m_treeItemModels;
    QTreeWidgetItem* m_treeItemGraphs;
    QHash<QString, AttrWidget*> m_attrWidgets;
    AttrWidget* m_enableOutputs;
    int m_graphTypeIdx;  // position of the graphtype combobox in the graph tree
    int m_edgesAttrsIdx; // position of the edgesAttrs field in the graph tree

    void addPluginAttrs(QTreeWidgetItem* tree, const Plugin* plugin);

    void addGeneralAttr(QTreeWidgetItem* itemRoot,
            const QString& attrName, QWidget *customWidget=nullptr);

    std::unique_ptr<ExpInputs> readInputs(const int expId, QString& error) const;

    void pluginSelected(QTreeWidgetItem* itemRoot, const PluginKey& key);
};
}

#endif // EXPERIMENT_DESIGNER_H
