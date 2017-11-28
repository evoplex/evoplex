/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QVBoxLayout>
#include <QSpacerItem>

#include "queuewidget.h"
#include "ui_queuewidget.h"

namespace evoplex {

QueueWidget::QueueWidget(ExperimentsMgr* expMgr, QWidget* parent)
    : QScrollArea(parent)
    , m_ui(new Ui_QueueWidget)
    , m_expMgr(expMgr)
{
    m_ui->setupUi(this);

    int col = 0;
    m_headerIdx.insert(TableWidget::H_BUTTON, col++);
    m_headerIdx.insert(TableWidget::H_PROJID, col++);
    m_headerIdx.insert(TableWidget::H_EXPID, col++);
    m_headerIdx.insert(TableWidget::H_STOPAT, col++);
    m_headerIdx.insert(TableWidget::H_TRIALS, col++);
    const QList<TableWidget::Header> header = m_headerIdx.keys();
    m_ui->tableRunning->insertColumns(header);
    m_ui->tableQueue->insertColumns(header);
    m_ui->tableIdle->insertColumns(header);

    m_ui->running->hide();
    m_ui->queue->hide();
    m_ui->idle->hide();

    connect(m_expMgr, SIGNAL(trialsDeleted(Experiment*)),
            this, SLOT(slotRemoveRow(Experiment*)));
    connect(m_expMgr, SIGNAL(statusChanged(Experiment*)),
            this, SLOT(slotStatusChanged(Experiment*)));
    connect(m_expMgr, SIGNAL(progressUpdated(Experiment*)),
            m_ui->tableRunning->viewport(), SLOT(update()));
    connect(m_ui->bClearQueue, SIGNAL(clicked(bool)),
            expMgr, SLOT(clearQueue()));
    connect(m_ui->bClearIdle, SIGNAL(clicked(bool)),
            expMgr, SLOT(clearIdle()));
    connect(m_ui->tableIdle, &QTableWidget::cellClicked, [this]() {
            m_ui->tableQueue->clearSelection(); m_ui->tableRunning->clearSelection();});
    connect(m_ui->tableQueue, &QTableWidget::cellClicked, [this]() {
            m_ui->tableIdle->clearSelection(); m_ui->tableRunning->clearSelection();});
    connect(m_ui->tableRunning, &QTableWidget::cellClicked, [this]() {
            m_ui->tableIdle->clearSelection(); m_ui->tableQueue->clearSelection();});
}

void QueueWidget::slotRemoveRow(Experiment *exp)
{
    Row row = m_rows.take(std::make_pair(exp->projId(), exp->id()));
    if (!row.table) {
        return;
    }
    row.table->removeRow(row.item->row());
    row.section->setVisible(row.table->rowCount() > 0);
    emit (isEmpty(!m_ui->idle->isVisible() && !m_ui->running->isVisible() && !m_ui->queue->isVisible()));
}

void QueueWidget::slotStatusChanged(Experiment* exp)
{
    const rowKey key = std::make_pair(exp->projId(), exp->id());
    Row prev = m_rows.value(key, Row());
    Row next = prev;

    switch (exp->expStatus()) {
        case Experiment::RUNNING:
            next.table = m_ui->tableRunning;
            next.section = m_ui->running;
            break;
        case Experiment::QUEUED:
            next.table = m_ui->tableQueue;
            next.section = m_ui->queue;
            break;
        default:
            next.table = m_ui->tableIdle;
            next.section = m_ui->idle;
    }

    next.section->show();

    if (prev.table == next.table) {
        return;
    } else if (prev.table) {
        moveRow(prev.table, prev.item->row(), next.table, exp);
        prev.section->setVisible(prev.table->rowCount() > 0);
    } else {
        next.item = insertRow(next.table, exp);
    }

    m_rows.insert(key, next);
    emit (isEmpty(false));
}

QTableWidgetItem* QueueWidget::insertRow(TableWidget* table, Experiment* exp)
{
    const int row = table->insertRow(exp);

    auto add = [this, table, row](TableWidget::Header header, int label) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(label));
        item->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, m_headerIdx.value(header), item);
        return item;
    };

    add(TableWidget::H_STOPAT, exp->stopAt());
    add(TableWidget::H_TRIALS, exp->numTrials());
    add(TableWidget::H_PROJID, exp->projId());
    return add(TableWidget::H_EXPID, exp->id());
}

void QueueWidget::moveRow(TableWidget* prevTable, int preRow, TableWidget* nextTable, Experiment* exp)
{
    const int nextRow = nextTable->insertRow(exp);
    const int cols = prevTable->columnCount();
    for (int col = 0; col < cols; ++col) {
        QTableWidgetItem* item = prevTable->takeItem(preRow, col);
        nextTable->setItem(nextRow, col, item);
    }
    prevTable->removeRow(preRow);
}
}
