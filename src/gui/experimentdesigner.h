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

#include <QCheckBox>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QVariantHash>
#include <QSpinBox>

#include "projectspage.h"
#include "core/project.h"

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

    void setExperiment(Experiment* exp);

private slots:
    void slotNodesWidget();
    void slotOutputDir();
    void slotOutputWidget();
    void slotCreateExperiment();
    void slotEditExperiment();
    void slotModelSelected(const QString& modelId);
    void slotGraphSelected(const QString& graphId);
    void slotPluginAdded(const AbstractPlugin* plugin);
    void slotPluginRemoved(const QString& id, AbstractPlugin::PluginType type);
    void slotSetActiveWidget(int idx);

private:
    MainApp* m_mainApp;
    ProjectPtr m_project;
    Experiment* m_exp;
    QString m_selectedGraphId;
    QString m_selectedModelId;

    Ui_ExperimentDesigner* m_ui;
    QTreeWidgetItem* m_treeItemGeneral;
    QTreeWidgetItem* m_treeItemOutputs;
    QTreeWidgetItem* m_treeItemModels;
    QTreeWidgetItem* m_treeItemGraphs;
    QVariantHash m_widgetFields;
    QCheckBox* m_enableOutputs;
    int m_customGraphIdx; // position of combobox in the graph tree

    void addPluginAttrs(QTreeWidgetItem* tree, const AbstractPlugin* plugin);

    QSpinBox* newSpinBox(const int min, const int max);
    QDoubleSpinBox* newDoubleSpinBox(const double min, const double max);
    void addTreeWidget(QTreeWidgetItem* itemRoot, const QString& label, const QVariant& widget);

    ExpInputs* readInputs(const int expId, QString& error) const;

    void pluginSelected(QTreeWidgetItem* itemRoot, const QString& pluginId);
};
}

#endif // EXPERIMENT_DESIGNER_H
