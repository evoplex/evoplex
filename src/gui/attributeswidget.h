/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ATTRIBUTESWIDGET_H
#define ATTRIBUTESWIDGET_H

#include <QCheckBox>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QVariantHash>
#include <QSpinBox>

#include "core/project.h"

class Ui_AttributesWidget;

namespace evoplex {

class AttributesWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit AttributesWidget(MainApp* mainApp, Project *project, QWidget *parent = 0);
    ~AttributesWidget();

    void setExperiment(Experiment* exp);

private slots:
    void slotAgentFile();
    void slotOutputDir();
    void slotOutputWidget();
    void slotCreateExperiment();
    void slotEditExperiment();
    void slotModelSelected(const QString& modelId);
    void slotGraphSelected(const QString& graphId);
    void slotPluginsUpdated(AbstractPlugin::PluginType type);

private:
    MainApp* m_mainApp;
    Project* m_project;
    Experiment* m_exp;
    QString m_selectedGraphId;
    QString m_selectedModelId;

    Ui_AttributesWidget* m_ui;
    QTreeWidgetItem* m_treeItemGeneral;
    QTreeWidgetItem* m_treeItemOutputs;
    QTreeWidgetItem* m_treeItemModels;
    QTreeWidgetItem* m_treeItemGraphs;
    QVariantHash m_widgetFields;
    QCheckBox* m_enableOutputs;
    int m_customGraphIdx; // position of combobox in the graph tree

    void insertPluginAttributes(QTreeWidgetItem* itemRoot, const QString &uid,
                                const AttributesSpace& attrsSpace);

    QSpinBox* newSpinBox(const int min, const int max);
    QDoubleSpinBox* newDoubleSpinBox(const double min, const double max);
    void addTreeWidget(QTreeWidgetItem* itemRoot, const QString& label, const QVariant& widget);

    Experiment::ExperimentInputs* readInputs();

    void pluginSelected(QTreeWidgetItem* itemRoot, const QString& pluginId);
};
}

#endif // ATTRIBUTESWIDGET_H
