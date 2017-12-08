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
    explicit AttributesWidget(Project *project, QWidget *parent = 0);
    ~AttributesWidget();

    void fill(Experiment* exp);

private slots:
    void slotAgentFile();
    void slotOutputDir();
    void slotOutputWidget();
    void slotCreateExperiment();
    void slotModelSelected(const QString& modelId);
    void slotGraphSelected(const QString& graphId);
    void slotUpdateModelPlugins();
    void slotUpdateGraphPlugins();

private:
    Project* m_project;
    QString m_selectedGraphId;
    QString m_selectedModelId;

    Ui_AttributesWidget* m_ui;
    QTreeWidgetItem* m_treeItemGeneral;
    QTreeWidgetItem* m_treeItemOutputs;
    QHash<QString, QTreeWidgetItem*> m_treeItemGraphs;
    QHash<QString, QTreeWidgetItem*> m_treeItemModels;
    QVariantHash m_widgetFields;
    QCheckBox* m_enableOutputs;

    void insertPluginAttributes(QTreeWidgetItem* itemRoot, const QString &uid,
                                const AttributesRange &range);

    QSpinBox* newSpinBox(const int min, const int max);
    QDoubleSpinBox* newDoubleSpinBox(const int min, const int max);
    void addTreeWidget(QTreeWidgetItem* itemRoot, const QString& label, const QVariant& widget);
};
}

#endif // ATTRIBUTESWIDGET_H
