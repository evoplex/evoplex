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
    , m_project(project)
{
    m_ui->setupUi(this);

    setObjectName(m_project->name());
    setWindowTitle(objectName());

    connect(m_project, SIGNAL(expAdded(int)), SLOT(slotInsertRow(int)));
    connect(m_project, SIGNAL(expEdited(int)), SLOT(slotUpdateRow(int)));

    int col = 0;
    m_headerIdx.insert(TableWidget::H_BUTTON, col++);
    m_headerIdx.insert(TableWidget::H_EXPID, col++);
    m_headerIdx.insert(TableWidget::H_SEED, col++);
    m_headerIdx.insert(TableWidget::H_STOPAT, col++);
    m_headerIdx.insert(TableWidget::H_MODEL, col++);
    m_headerIdx.insert(TableWidget::H_GRAPH, col++);
    m_headerIdx.insert(TableWidget::H_TRIALS, col++);
    m_ui->table->insertColumns(m_headerIdx.keys());

    connect(m_ui->playAll, &QPushButton::pressed, [this]() { m_project->playAll(); });

    connect(m_ui->table, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));
    connect(m_ui->table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            SLOT(onItemDoubleClicked(QTableWidgetItem*)));

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
}

void ProjectWidget::closeEvent(QCloseEvent* event)
{
    if (m_project->hasUnsavedChanges()) {
        QMessageBox::StandardButton res = QMessageBox::question(this, "Evoplex",
                tr("Do you want to save the changes you made to '%1'?\n"
                   "Your changes will be lost if you don’t save them.").arg(m_project->name()),
                QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save, QMessageBox::Save);

        if (res == QMessageBox::Cancel || (res == QMessageBox::Save && !m_mainGUI->saveDialog()->save(m_project))) {
            event->ignore();
            return;
        }
    }
    emit (closed());
    event->accept();
    QDockWidget::closeEvent(event);
}

void ProjectWidget::fillRow(int row, Experiment* exp)
{
    Q_ASSERT(exp);

    m_ui->table->setSortingEnabled(false);

    // general stuff
    const Attributes* gep = exp->generalAttrs();
    insertItem(row, TableWidget::H_EXPID, QString::number(exp->id()));
    insertItem(row, TableWidget::H_SEED, gep->value(GENERAL_ATTRIBUTE_SEED).toQString());
    insertItem(row, TableWidget::H_STOPAT, gep->value(GENERAL_ATTRIBUTE_STOPAT).toQString());
    insertItem(row, TableWidget::H_TRIALS, gep->value(GENERAL_ATTRIBUTE_TRIALS).toQString());

    // lambda function to add the attributes of a plugin (ie, model or graph)
    auto pluginAtbs = [this, row](TableWidget::Header header, QString pluginId, const Attributes* attrs)
    {
        QString pluginAttrs = pluginId;
        for (const Value& v : attrs->values()) {
            pluginAttrs += QString(" | %1").arg(v.toQString());
        }
        QTableWidgetItem* item = new QTableWidgetItem(pluginAttrs);
        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(pluginAttrs);
        //QFont font = item->font();
        //font.setItalic(true);
        //item->setFont(font);
        m_ui->table->setItem(row, m_headerIdx.value(header), item);
    };

    // model stuff
    pluginAtbs(TableWidget::H_MODEL, exp->modelId(), exp->modelAttrs());

    // graph stuff
    pluginAtbs(TableWidget::H_GRAPH, exp->graphId(), exp->graphAttrs());

    m_ui->table->setSortingEnabled(true);
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

void ProjectWidget::slotSelectionChanged()
{
    Experiment* exp = nullptr;
    if (!m_ui->table->selectedItems().isEmpty()) {
        int row = m_ui->table->selectedItems().first()->row();
        int expId = m_ui->table->item(row, m_headerIdx.value(TableWidget::H_EXPID))->text().toInt();
        exp = m_project->experiment(expId);
    }
    emit (expSelectionChanged(exp));
}

void ProjectWidget::onItemDoubleClicked(QTableWidgetItem* item)
{
    int expId = m_ui->table->item(item->row(), m_headerIdx.value(TableWidget::H_EXPID))->text().toInt();
    emit (openExperiment(m_project->experiment(expId)));
}

void ProjectWidget::slotHasUnsavedChanges(bool b)
{
    setObjectName(m_project->name());
    setWindowTitle(objectName() + (b ? "*" : ""));
    emit (hasUnsavedChanges(m_project));
}

void ProjectWidget::clearSelection()
{
    m_ui->table->clearSelection();
}

} // evoplex
