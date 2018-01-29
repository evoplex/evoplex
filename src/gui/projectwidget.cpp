/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>

#include "projectwidget.h"
#include "savedialog.h"
#include "ui_projectwidget.h"
#include "core/experimentsmgr.h"

namespace evoplex {

ProjectWidget::ProjectWidget(MainGUI* mainGUI, Project* project, ProjectsPage* ppage)
    : QDockWidget(ppage)
    , m_ui(new Ui_ProjectWidget)
    , m_mainGUI(mainGUI)
    , m_innerWindow(new QMainWindow(this, Qt::FramelessWindowHint))
    , m_attrWidget(new AttributesWidget(mainGUI->mainApp(), project, m_innerWindow))
    , m_project(project)
{
    setObjectName(m_project->name());
    setWindowTitle(objectName());
    setAttribute(Qt::WA_DeleteOnClose, true);

    QHBoxLayout* lh = new QHBoxLayout(new QWidget(this));
    lh->addWidget(m_innerWindow);
    this->setWidget(lh->parentWidget());

    QWidget* projectWidget = new QWidget(m_innerWindow);
    m_ui->setupUi(projectWidget);
    m_innerWindow->setCentralWidget(projectWidget);
    m_innerWindow->setStyleSheet("QMainWindow { background-color: rgb(24,24,24); }");

    m_innerWindow->addDockWidget(Qt::RightDockWidgetArea, m_attrWidget);
    m_attrWidget->setTitleBarWidget(new QWidget());
    m_attrWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    connect(m_project, SIGNAL(expAdded(int)), SLOT(slotInsertRow(int)));
    connect(m_project, SIGNAL(expEdited(int)), SLOT(slotUpdateRow(int)));

    int col = 0;
    m_headerIdx.insert(TableWidget::H_BUTTON, col++);
    m_headerIdx.insert(TableWidget::H_EXPID, col++);
    m_headerIdx.insert(TableWidget::H_SEED, col++);
    m_headerIdx.insert(TableWidget::H_STOPAT, col++);
    m_headerIdx.insert(TableWidget::H_AGENTS, col++);
    m_headerIdx.insert(TableWidget::H_MODEL, col++);
    m_headerIdx.insert(TableWidget::H_GRAPH, col++);
    m_headerIdx.insert(TableWidget::H_TRIALS, col++);
    m_ui->table->insertColumns(m_headerIdx.keys());

    connect(m_ui->playAll, &QPushButton::pressed, [this]() { m_project->playAll(); });

    connect(m_ui->table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            SLOT(onItemDoubleClicked(QTableWidgetItem*)));
    connect(m_ui->table, SIGNAL(itemClicked(QTableWidgetItem*)),
            SLOT(onItemClicked(QTableWidgetItem*)));

    connect(m_mainGUI->mainApp()->expMgr(), SIGNAL(statusChanged(Experiment*)),
            m_ui->table->viewport(), SLOT(update()));
    connect(m_mainGUI->mainApp()->expMgr(), SIGNAL(progressUpdated(Experiment*)),
            m_ui->table->viewport(), SLOT(update()));

    connect(project, SIGNAL(hasUnsavedChanges(bool)),
            SLOT(slotHasUnsavedChanges(bool)));
}

ProjectWidget::~ProjectWidget()
{
    delete m_ui;
    delete m_innerWindow;
}

void ProjectWidget::closeEvent(QCloseEvent* event)
{
    if (m_project->hasUnsavedChanges()) {
        QMessageBox::StandardButton res = QMessageBox::question(this, "Save Project?",
                tr("Save project '%1' before closing?\nYour changes will be lost if you don’t save them.").arg(m_project->name()),
                QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save, QMessageBox::Save);

        if (res == QMessageBox::Cancel || (res == QMessageBox::Save && !m_mainGUI->saveDialog()->save(m_project))) {
            event->ignore();
            return;
        }
    }
    event->accept();
    QDockWidget::closeEvent(event);
}

void ProjectWidget::fillRow(int row, Experiment* exp)
{
    Q_ASSERT(exp);

    // general stuff
    const Attributes* gep = exp->generalAttrs();
    insertItem(row, TableWidget::H_EXPID, QString::number(exp->id()));
    insertItem(row, TableWidget::H_SEED, gep->value(GENERAL_ATTRIBUTE_SEED).toQString());
    insertItem(row, TableWidget::H_STOPAT, gep->value(GENERAL_ATTRIBUTE_STOPAT).toQString());
    insertItem(row, TableWidget::H_TRIALS, gep->value(GENERAL_ATTRIBUTE_TRIALS).toQString());
    bool isRandom;
    gep->value(GENERAL_ATTRIBUTE_AGENTS).toQString().toInt(&isRandom);
    if (isRandom)
        insertItem(row, TableWidget::H_AGENTS, "R", "Agents with random attributes.");
    else
        insertItem(row, TableWidget::H_AGENTS, "F", "Agents from file.");

    // lambda function to add the attributes of a plugin (ie, model or graph)
    auto pluginAtbs = [this, row](TableWidget::Header header, QString pluginId, const Attributes* attrs)
    {
        QString pluginAttrs = "id:" + pluginId;
        pluginId += "_";
        for (int i = 0; i < attrs->size(); ++i) {
            pluginAttrs += QString(" | %1:%2")
                    .arg(QString(attrs->name(i)).remove(pluginId))
                    .arg(attrs->value(i).toQString());
        }
        QTableWidgetItem* item = new QTableWidgetItem(pluginAttrs);
        item->setTextAlignment(Qt::AlignCenter);
        QFont font = item->font();
        font.setItalic(true);
        item->setFont(font);
        m_ui->table->setItem(row, m_headerIdx.value(header), item);
    };

    // model stuff
    pluginAtbs(TableWidget::H_MODEL, exp->modelId(), exp->modelAttrs());

    // graph stuff
    pluginAtbs(TableWidget::H_GRAPH, exp->graphId(), exp->graphAttrs());
}

void ProjectWidget::slotInsertRow(int expId)
{
    Experiment* exp = m_project->experiment(expId);
    fillRow(m_ui->table->insertRow(exp), exp);
}

void ProjectWidget::slotUpdateRow(int expId)
{
    const int expIdCol = m_headerIdx.value(TableWidget::H_EXPID);
    for (int row = 0; row < m_ui->table->rowCount(); ++row) {
        if (expId == m_ui->table->item(row, expIdCol)->text().toInt()) {
            fillRow(row, m_project->experiment(expId));
            return;
        }
    }
    qFatal("[Experiment]: failed to update row! It should never happen.");
}

void ProjectWidget::insertItem(int row, TableWidget::Header header, QString label, QString tooltip)
{
    QTableWidgetItem* item = new QTableWidgetItem(label);
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(tooltip);
    m_ui->table->setItem(row, m_headerIdx.value(header), item);
}

void ProjectWidget::onItemClicked(QTableWidgetItem* item)
{
    if (m_ui->table->selectedItems().isEmpty()) {
        m_attrWidget->setExperiment(nullptr);
        return;
    }

    int expId = m_ui->table->item(item->row(), m_headerIdx.value(TableWidget::H_EXPID))->text().toInt();
    Experiment* exp = m_project->experiment(expId);
    m_attrWidget->setExperiment(exp);
}

void ProjectWidget::onItemDoubleClicked(QTableWidgetItem* item)
{
    int expId = m_ui->table->item(item->row(), m_headerIdx.value(TableWidget::H_EXPID))->text().toInt();
    emit (openExperiment(m_project->id(), expId));
}

void ProjectWidget::slotHasUnsavedChanges(bool b)
{
    setWindowTitle(objectName() + (b ? "*" : ""));
    emit (hasUnsavedChanges(this));
}
}
