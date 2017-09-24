#ifndef ATTRIBUTESWIDGET_H
#define ATTRIBUTESWIDGET_H

#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QVariantHash>

#include "core/project.h"

class Ui_AttributesWidget;

namespace evoplex {

class AttributesWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit AttributesWidget(Project *project, QWidget *parent = 0);
    ~AttributesWidget();

signals:
    void experimentCreated(const int& expId);

private slots:
    void slotAgentFile();
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
    QHash<QString, QTreeWidgetItem*> m_treeItemGraphs;
    QHash<QString, QTreeWidgetItem*> m_treeItemModels;
    QVariantHash m_widgetFields;

    void insertPluginAttributes(QTreeWidgetItem* itemRoot, const QString &uid,
                                const Attributes &min, const Attributes &max);
};
}

#endif // ATTRIBUTESWIDGET_H
