/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

ProjectWidget::ProjectWidget(ProjectPtr project, MainGUI* mainGUI, ProjectsPage* ppage)
    : PPageDockWidget(ppage)
    , m_ui(new Ui_ProjectWidget)
    , m_mainGUI(mainGUI)
    , m_project(project)
{
    m_ui->setupUi(this);

    setObjectName(m_project->name());
    setWindowTitle(objectName());
    setFocusPolicy(Qt::StrongFocus);

    connect(m_project.data(), SIGNAL(expAdded(Experiment*)), SLOT(slotInsertRow(Experiment*)));
    connect(m_project.data(), SIGNAL(expEdited(const Experiment*)), SLOT(slotUpdateRow(const Experiment*)));

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

    connect(project.data(), SIGNAL(hasUnsavedChanges(bool)),
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

void ProjectWidget::fillRow(int row, const Experiment* exp)
{
    Q_ASSERT(exp && exp->inputs());

    m_ui->table->setSortingEnabled(false);

    // general stuff
    insertItem(row, TableWidget::H_EXPID, QString::number(exp->id()));
    insertItem(row, TableWidget::H_SEED, exp->inputs()->general(GENERAL_ATTRIBUTE_SEED).toQString());
    insertItem(row, TableWidget::H_STOPAT, exp->inputs()->general(GENERAL_ATTRIBUTE_STOPAT).toQString());
    insertItem(row, TableWidget::H_TRIALS, exp->inputs()->general(GENERAL_ATTRIBUTE_TRIALS).toQString());

    if (exp->expStatus() == Experiment::INVALID) {
        m_ui->table->setSortingEnabled(true);
        return;
    }

    // lambda function to add the attributes of a plugin (ie, model or graph)
    auto pluginAtbs = [this, row](TableWidget::Header header, const QString& pluginId, const Attributes* attrs)
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
    pluginAtbs(TableWidget::H_MODEL, exp->modelId(), exp->inputs()->model());

    // graph stuff
    pluginAtbs(TableWidget::H_GRAPH, exp->graphId(), exp->inputs()->graph());

    m_ui->table->setSortingEnabled(true);
}

void ProjectWidget::slotInsertRow(Experiment* exp)
{
    fillRow(m_ui->table->insertRow(exp), exp);
}

void ProjectWidget::slotUpdateRow(const Experiment* exp)
{
    const int expIdCol = m_headerIdx.value(TableWidget::H_EXPID);
    for (int row = 0; row < m_ui->table->rowCount(); ++row) {
        if (exp->id() == m_ui->table->item(row, expIdCol)->text().toInt()) {
            fillRow(row, exp);
            return;
        }
    }
    qFatal("failed to update row! It should never happen.");
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
