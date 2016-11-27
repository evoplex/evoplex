/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFileDialog>
#include <QHashIterator>
#include <QMessageBox>
#include <QtDebug>

#include "gui/maingui.h"
#include "gui/widgetproject.h"
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

    const QString treeFieldStyle = "*{background-color: rgba(0,0,0,0);}";

    //
    // simulation settings
    //
    QTreeWidgetItem* item;
    QTreeWidgetItem* general = new QTreeWidgetItem(m_ui->treeSettings);
    general->setText(0, "Simulation");

    item = new QTreeWidgetItem(general);
    item->setText(0, GENERAL_PROPERTY_NAME_SEED);
    m_treeSeed = new QSpinBox();
    m_treeSeed->setMaximum(100000000);
    m_treeSeed->setFrame(false);
    m_treeSeed->setStyleSheet(treeFieldStyle);
    m_treeSeed->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_ui->treeSettings->setItemWidget(item, 1, m_treeSeed);

    item =new QTreeWidgetItem(general);
    item->setText(0, GENERAL_PROPERTY_NAME_STOPAT);
    m_treeStopAt = new QSpinBox();
    m_treeStopAt->setMaximum(100000000);
    m_treeStopAt->setValue(10000);
    m_treeStopAt->setFrame(false);
    m_treeStopAt->setStyleSheet(treeFieldStyle);
    m_treeStopAt->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_ui->treeSettings->setItemWidget(item, 1, m_treeStopAt);

    //
    // population settings
    //
    QTreeWidgetItem* env = new QTreeWidgetItem(m_ui->treeSettings);
    env->setText(0, "Population");

    item =new QTreeWidgetItem(env);
    item->setText(0, GENERAL_PROPERTY_NAME_AGENTS);
    m_treeAgents = new QLineEdit(m_project->getDir());
    m_treeAgents->setFrame(false);
    m_treeAgents->setStyleSheet(treeFieldStyle);
    m_ui->treeSettings->setItemWidget(item, 1, m_treeAgents);

    item =new QTreeWidgetItem(env);
    item->setText(0, GENERAL_PROPERTY_NAME_GRAPHTYPE);
    m_treeGraphType = new QComboBox();
    m_treeGraphType->insertItem(0, "Square Grid (k=4)",
            Graph::graphTypeToString(Graph::SQUARE_GRID));
    m_treeGraphType->insertItem(1, "Moore Grid (k=8)",
            Graph::graphTypeToString(Graph::MOORE_GRID));
    m_treeGraphType->setFrame(false);
    m_ui->treeSettings->setItemWidget(item, 1, m_treeGraphType);

    //
    // model parameters
    //
    QTreeWidgetItem* params = new QTreeWidgetItem(m_ui->treeSettings);
    params->setText(0, "Model");
    QHashIterator<QString, QVariant> i(project->getModel()->defaultModelParams);
    while (i.hasNext()) {
        i.next();
        QString propertyName = i.key();
        QVariant propertyValue = i.value();

        item = new QTreeWidgetItem(params);
        item->setText(0, propertyName);
        QDoubleSpinBox* box = new QDoubleSpinBox();
        box->setDecimals(4);
        box->setValue(propertyValue.toDouble());
        box->setFrame(false);
        box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        box->setStyleSheet(treeFieldStyle);
        m_treeModelParams.insert(propertyName, box);
        m_ui->treeSettings->setItemWidget(item, 1, box);
    }

    //
    // setup tree widget
    //
    m_ui->treeSettings->expandAll();

    //
    // setup table of experiments
    //
    QStringList tableHeader;
    tableHeader << STRING_EXPERIMENT_ID
                << GENERAL_PROPERTY_NAME_SEED
                << GENERAL_PROPERTY_NAME_STOPAT
                << GENERAL_PROPERTY_NAME_AGENTS
                << m_project->getModel()->defaultModelParams.keys()
                << STRING_PROCESS_STATUS;

    m_ui->tableExperiments->setColumnCount(tableHeader.size());
    m_ui->tableExperiments->setHorizontalHeaderLabels(tableHeader);
    for (int col = 0; col < tableHeader.size(); ++col) {
        m_tableHeader.insert(tableHeader.at(col), col);
    }
}

WidgetProject::~WidgetProject()
{
    delete m_treeGraphType;
    m_treeGraphType = NULL;
    delete m_treeAgents;
    m_treeAgents = NULL;
    delete m_treeSeed;
    m_treeSeed = NULL;
    delete m_treeStopAt;
    m_treeStopAt = NULL;
    qDeleteAll(m_treeModelParams);
    delete m_ui;
    m_ui = NULL;
}

void WidgetProject::insertRow(Simulation* sim, QVariantHash generalParams, QVariantHash modelParams)
{
    generalParams.insert(STRING_EXPERIMENT_ID, sim->getExperimentId());
    generalParams.insert(STRING_PROCESS_STATUS, sim->getStatus());

    int row = m_ui->tableExperiments->rowCount();
    m_ui->tableExperiments->insertRow(row);
    for (int col = 0; col < m_ui->tableExperiments->columnCount(); ++col) {
        QString name = m_ui->tableExperiments->horizontalHeaderItem(col)->text();
        QVariant value = generalParams.value(name, QVariant());
        value = value.isNull() ? modelParams.value(name, QVariant()) : value;
        QString valueStr = value.isNull() ? "" : value.toString();
        m_ui->tableExperiments->setItem(row, col, new QTableWidgetItem(valueStr));
    }
}

void WidgetProject::slotStatusChanged(int experimentId, int processId, int newStatus)
{
    Q_UNUSED(processId);
    // find row with the experimentId
    int colId = m_tableHeader.value(STRING_EXPERIMENT_ID);
    QAbstractItemModel* model = m_ui->tableExperiments->model();
    QModelIndexList r = model->match(model->index(experimentId, colId),
                                     Qt::DisplayRole, QString::number(experimentId));
    if (r.isEmpty()) {
        qWarning() << "[WidgetProject]: tried to update the status of a non-existent experiment!";
        return;
    }

    // set the status
    int colStatus = m_tableHeader.value(STRING_PROCESS_STATUS);
    QTableWidgetItem* s = m_ui->tableExperiments->item(r.first().row(), colStatus);
    s->setText(qobject_cast<MainGUI*>(parentWidget())->statusToString((Simulation::Status) newStatus));
    s->setData(Qt::UserRole, newStatus);
}

int WidgetProject::slotAdd()
{
    QVariantHash gp = getGeneralParams();
    QVariantHash mp = getModelParams();

    Simulation* sim = m_project->newExperiment(gp, mp);
    if (!sim) {
        QMessageBox::warning(this, "New Experiment", "Unable to load a new experiment!");
        return -1;
    }

    // let the table aware of any status change
    connect(sim, SIGNAL(statusChanged(int, int, int)),
            this, SLOT(slotStatusChanged(int, int, int)));

    // update table of experiments
    insertRow(sim, gp, mp);
    return sim->getExperimentId();
}

void WidgetProject::slotAddRun()
{
    m_project->runExperiment(slotAdd());
}

void WidgetProject::slotImport()
{
    QString filePath = QFileDialog::getOpenFileName(this,
            "Import Experiments", m_treeAgents->text(), "Text Files (*.csv *.txt)");

    if (filePath.isEmpty()) {
        return;
    }

    QVariantHash gp;
    QVariantHash mp;
    QList<Simulation*> sims = m_project->importExperiments(filePath, gp, mp);
    foreach (Simulation* sim, sims) {
        insertRow(sim, gp, mp);
    }
}

void WidgetProject::slotRunAll()
{

}

void WidgetProject::slotRunSelected()
{

}

QVariantHash WidgetProject::getGeneralParams()
{
    QVariantHash generalParams;
    generalParams.insert(GENERAL_PROPERTY_NAME_AGENTS, m_treeAgents->text());
    generalParams.insert(GENERAL_PROPERTY_NAME_GRAPHTYPE, m_treeGraphType->currentData());
    generalParams.insert(GENERAL_PROPERTY_NAME_SEED, m_treeSeed->value());
    generalParams.insert(GENERAL_PROPERTY_NAME_STOPAT, m_treeStopAt->value());
    return generalParams;
}

QVariantHash WidgetProject::getModelParams()
{
    QVariantHash modelParams;
    QHashIterator<QString, QDoubleSpinBox*> i(m_treeModelParams);
    while (i.hasNext()) {
        i.next();
        modelParams.insert(i.key(), i.value()->text());
    }
    return modelParams;
}
