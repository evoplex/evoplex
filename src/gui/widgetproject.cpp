#include <QMapIterator>
#include <QtDebug>

#include "widgetproject.h"
#include "ui_widgetproject.h"

WidgetProject::WidgetProject(Project* project, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::WidgetProject)
    , m_project(project)
{
    m_ui->setupUi(this);

    connect(m_ui->bAdd, SIGNAL(clicked(bool)), this, SLOT(slotAdd()));
    connect(m_ui->bAddRun, SIGNAL(clicked(bool)), this, SLOT(slotAddRun()));
    connect(m_ui->bImport, SIGNAL(clicked(bool)), this, SLOT(slotImport()));
    connect(m_ui->bRunAll, SIGNAL(clicked(bool)), this, SLOT(slotRunAll()));
    connect(m_ui->bRunSelected, SIGNAL(clicked(bool)), this, SLOT(slotRunSelected()));

    //
    // simulation settings
    //
    QTreeWidgetItem* general = new QTreeWidgetItem(m_ui->treeSettings);
    general->setText(0, "Simulation");

    QTreeWidgetItem* item = new QTreeWidgetItem(general);
    item->setText(0, "Seed");
    m_treeSeed = new QSpinBox();
    m_ui->treeSettings->setItemWidget(item, 1, m_treeSeed);

    item =new QTreeWidgetItem(general);
    item->setText(0, "Stop at");
    m_treeStopAt = new QSpinBox();
    m_ui->treeSettings->setItemWidget(item, 1, m_treeStopAt);

    //
    // population settings
    //
    QTreeWidgetItem* env = new QTreeWidgetItem(m_ui->treeSettings);
    env->setText(0, "Environment");

    item =new QTreeWidgetItem(env);
    item->setText(0, "Type");
    m_treeGraphType = new QComboBox();
    m_treeGraphType->insertItem(Graph::SQUARE_GRID, "Square Grid (k=4)");
    m_treeGraphType->insertItem(Graph::MOORE_GRID, "Moore Grid (k=8)");
    m_ui->treeSettings->setItemWidget(item, 1, m_treeGraphType);

    item =new QTreeWidgetItem(env);
    item->setText(0, "Initial Population");
    m_treeInitialPop = new QLineEdit(m_project->getDir());
    m_ui->treeSettings->setItemWidget(item, 1, m_treeInitialPop);

    //
    // model parameters
    //
    QTreeWidgetItem* params = new QTreeWidgetItem(m_ui->treeSettings);
    params->setText(0, "Model");
    QMapIterator<QString, QVariant> i(project->getModel()->params);
    while (i.hasNext()) {
        i.next();
        item = new QTreeWidgetItem(params);
        item->setText(0, i.key());
        QDoubleSpinBox* box = new QDoubleSpinBox();
        box->setDecimals(6);
        box->setValue(i.value().toDouble());
        m_treeModelParams.insert(i.key(), box);
        m_ui->treeSettings->setItemWidget(item, 1, box);
    }
}

WidgetProject::~WidgetProject()
{
    delete m_ui;
    m_ui = NULL;
}

void WidgetProject::slotAdd()
{
    Graph::GraphType gtype = (Graph::GraphType) m_treeGraphType->currentIndex();
    Graph* g = new Graph(m_treeInitialPop->text(), gtype);

    QVariantMap params;
    QMapIterator<QString, QDoubleSpinBox*> i(m_treeModelParams);
    while (i.hasNext()) {
        i.next();
        params.insert(i.key(), i.value()->value());
    }

    m_project->newExperiment(g, params);
}

void WidgetProject::slotAddRun()
{

}

void WidgetProject::slotImport()
{

}

void WidgetProject::slotRunAll()
{

}

void WidgetProject::slotRunSelected()
{

}
