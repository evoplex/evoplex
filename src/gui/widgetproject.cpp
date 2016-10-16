#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include "widgetproject.h"
#include "ui_widgetproject.h"

WidgetProject::WidgetProject(Project* project, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::WidgetProject)
    , m_project(project)
{
    m_ui->setupUi(this);

    //
    // simulation settings
    //
    QTreeWidgetItem* general = new QTreeWidgetItem(m_ui->treeSettings);
    general->setText(0, "Simulation");

    QTreeWidgetItem* item = new QTreeWidgetItem(general);
    item->setText(0, "Seed");
    m_ui->treeSettings->setItemWidget(item, 1, new QSpinBox());

    item =new QTreeWidgetItem(general);
    item->setText(0, "Stop at");
    m_ui->treeSettings->setItemWidget(item, 1, new QSpinBox());

    //
    // population settings
    //
    QTreeWidgetItem* env = new QTreeWidgetItem(m_ui->treeSettings);
    env->setText(0, "Environment");

    item =new QTreeWidgetItem(env);
    item->setText(0, "Type");
    QComboBox* cb = new QComboBox();
    cb->insertItem(Graph::SQUARE_GRID, "Square Grid (k=4)");
    cb->insertItem(Graph::MOORE_GRID, "Moore Grid (k=8)");
    m_ui->treeSettings->setItemWidget(item, 1, cb);

    item =new QTreeWidgetItem(env);
    item->setText(0, "Initial Population");
    m_ui->treeSettings->setItemWidget(item, 1, new QLineEdit(m_project->getDir()));

    //
    // model parameters
    //
    QTreeWidgetItem* params = new QTreeWidgetItem(m_ui->treeSettings);
    params->setText(0, "Model");
    foreach (QMetaProperty mp, project->getModel()->params) {
        item = new QTreeWidgetItem(params);
        item->setText(0, mp.name());
        QDoubleSpinBox* box = new QDoubleSpinBox();
        box->setDecimals(6);
        m_ui->treeSettings->setItemWidget(item, 1, box);
    }
}

WidgetProject::~WidgetProject()
{
    delete m_ui;
    m_ui = NULL;
}


