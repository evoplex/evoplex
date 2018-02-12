/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
    explicit ExperimentDesigner(MainApp* mainApp, QWidget* parent = 0);
    ~ExperimentDesigner();

    void addWidgetToList(PPageDockWidget* dw);
    void removeWidgetFromList(PPageDockWidget* dw);
    void setActiveWidget(PPageDockWidget* dw, Project* project);

    void setExperiment(Experiment* exp);

private slots:
    void slotAgentsWidget();
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
    Project* m_project;
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

    Experiment::ExperimentInputs* readInputs();

    void pluginSelected(QTreeWidgetItem* itemRoot, const QString& pluginId);
};
}

#endif // EXPERIMENT_DESIGNER_H
