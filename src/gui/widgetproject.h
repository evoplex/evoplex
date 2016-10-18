#ifndef WIDGETPROJECT_H
#define WIDGETPROJECT_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMap>
#include <QSpinBox>
#include <QWidget>

#include "core/project.h"

namespace Ui {
class WidgetProject;
}

class WidgetProject : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetProject(Project* project, QWidget *parent = 0);
    ~WidgetProject();

private slots:
    void slotAdd();
    void slotAddRun();
    void slotImport();
    void slotRunAll();
    void slotRunSelected();

private:
    Ui::WidgetProject* m_ui;
    Project* m_project;

    QComboBox* m_treeGraphType;
    QLineEdit* m_treeInitialPop;
    QSpinBox* m_treeSeed;
    QSpinBox* m_treeStopAt;
    QMap<QString, QDoubleSpinBox*> m_treeModelParams;
};

#endif // WIDGETPROJECT_H
