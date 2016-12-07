/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/project.h"
#include "gui/maingui.h"
#include "gui/widgetproject.h"
#include "gui/wizardnewproject.h"
#include "ui_maingui.h"

MainGUI::MainGUI(MainApp* mainApp, QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainGUI)
    , m_mainApp(mainApp)
    , m_wizardNewProject(new WizardNewProject(m_mainApp, this))
{
    m_ui->setupUi(this);

    connect(m_ui->actionNewProject, SIGNAL(triggered(bool)), m_wizardNewProject, SLOT(show()));
    connect(m_mainApp->getProcessesMgr(), SIGNAL(newProcess(int)), this, SLOT(slotAddProcess(int)));

    // setup the context menu
    m_contextMenu = new ContextMenuTable(m_mainApp, m_ui->tableProcesses);
    m_ui->tableProcesses->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->tableProcesses, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));

    // setup table of processes
    QStringList tableHeader;
    tableHeader << STRING_PROCESS_ID
                << STRING_MODEL_NAME
                << STRING_PROJECT_NAME
                << STRING_EXPERIMENT_ID
                << STRING_PROCESS_STATUS;
    m_ui->tableProcesses->setColumnCount(tableHeader.size());
    m_ui->tableProcesses->setHorizontalHeaderLabels(tableHeader);
    for (int col = 0; col < tableHeader.size(); ++col) {
        m_tableHeader.insert(tableHeader.at(col), col);
    }
}

MainGUI::~MainGUI()
{
    delete m_contextMenu;
    m_contextMenu = NULL;
    delete m_ui;
    m_ui = NULL;
}

void MainGUI::slotContextMenu(QPoint point)
{
    const int row = m_ui->tableProcesses->rowAt(point.y());
    if (row == -1) {
        return;
    }

    QTableWidgetItem* ip = m_ui->tableProcesses->item(row, m_tableHeader.value(STRING_PROCESS_ID));
    QTableWidgetItem* is = m_ui->tableProcesses->item(row, m_tableHeader.value(STRING_PROCESS_STATUS));
    if (!ip || !is) {
        return;
    }

    point = m_ui->tableProcesses->mapToGlobal(point);
    int processId = ip->data(Qt::DisplayRole).toInt();
    Simulation::Status status = (Simulation::Status) is->data(Qt::UserRole).toInt();
    m_contextMenu->openMenu(point, processId, status);
}

void MainGUI::addProject(int projId)
{
    Project* p = m_mainApp->getProject(projId);
    WidgetProject* wp = new WidgetProject(m_mainApp, p, m_ui->tableProcesses);
    connect(m_contextMenu, SIGNAL(openView(int)), wp, SLOT(slotOpenView(int)));
    m_ui->tabWidget->addTab(wp, p->getName());
    m_ui->tabWidget->setCurrentWidget(wp);
}

void MainGUI::slotAddProcess(int processId)
{
    Simulation* s = m_mainApp->getProcessesMgr()->getProcess(processId);
    Project* p = m_mainApp->getProject(s->getProjectId());
    MainApp::Model* m = m_mainApp->getModel(s->getModelId());

    // let the table aware of any status change
    connect(s, SIGNAL(statusChanged(int, int, int)),
            this, SLOT(slotStatusChanged(int, int, int)));

    int row = m_ui->tableProcesses->rowCount();
    m_ui->tableProcesses->insertRow(row);

    m_ui->tableProcesses->setItem(row, m_tableHeader.value(STRING_PROCESS_ID),
                                  new QTableWidgetItem(QString::number(processId)));
    m_ui->tableProcesses->setItem(row, m_tableHeader.value(STRING_MODEL_NAME),
                                  new QTableWidgetItem(m->name));
    m_ui->tableProcesses->setItem(row, m_tableHeader.value(STRING_PROJECT_NAME),
                                  new QTableWidgetItem(p->getName()));
    m_ui->tableProcesses->setItem(row, m_tableHeader.value(STRING_EXPERIMENT_ID),
                                  new QTableWidgetItem(QString::number(s->getExperimentId())));

    QTableWidgetItem* status = new QTableWidgetItem();
    status->setData(Qt::DisplayRole, statusToString(s->getStatus()));
    status->setData(Qt::UserRole, s->getStatus());
    m_ui->tableProcesses->setItem(row, m_tableHeader.value(STRING_PROCESS_STATUS), status);
}

void MainGUI::slotStatusChanged(int experimentId, int processId, int newStatus)
{
    Q_UNUSED(experimentId);
    // find row with the processId
    int colId = m_tableHeader.value(STRING_PROCESS_ID);
    QAbstractItemModel* model = m_ui->tableProcesses->model();
    QModelIndexList r = model->match(model->index(processId, colId),
                                     Qt::DisplayRole, QString::number(processId));
    if (r.isEmpty()) {
        qWarning() << "[MainGUI]: tried to update the status of a non-existent experiment!";
        return;
    }

    // set the status
    int colStatus = m_tableHeader.value(STRING_PROCESS_STATUS);
    QTableWidgetItem* s = m_ui->tableProcesses->item(r.first().row(), colStatus);
    s->setText(statusToString((Simulation::Status) newStatus));
    s->setData(Qt::UserRole, newStatus);
}

const QString MainGUI::statusToString(Simulation::Status status) const
{
    if (status == Simulation::INVALID) return "Invalid";
    if (status == Simulation::READY) return "Idle";
    if (status == Simulation::RUNNING) return "Running";
    if (status == Simulation::FINISHING) return "Finishing";
    if (status == Simulation::FINISHED) return "Finished";
    return "Error!";
}
