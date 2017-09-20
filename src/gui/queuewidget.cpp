/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QVBoxLayout>
#include <QSpacerItem>

#include "queuewidget.h"

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

    connect(m_expMgr, SIGNAL(statusChanged(Experiment*)),
            this, SLOT(slotStatusChanged(Experiment*)));
    connect(m_expMgr, SIGNAL(progressUpdated(Experiment*)),
            m_ui->tableRunning->viewport(), SLOT(update()));
}

void QueueWidget::slotStatusChanged(Experiment* exp)
{
    const QString key = QString("%1.%2").arg(exp->getProjId()).arg(exp->getId());
    Row prev = m_rows.value(key, Row());
    Row next = prev;

    switch (exp->getExpStatus()) {
        case Experiment::RUNNING:
            if (prev.table != T_RUNNING) {
                next.item = insertRow(m_ui->tableRunning, exp);
                next.table = T_RUNNING;
                m_ui->running->show();
            }
            break;
        case Experiment::QUEUED:
            if (prev.table != T_QUEUED) {
                next.item = insertRow(m_ui->tableQueue, exp);
                next.table = T_QUEUED;
                m_ui->queue->show();
            }
            break;
        default:
            if (prev.table != T_IDLE) {
                next.item = insertRow(m_ui->tableIdle, exp);
                next.table = T_IDLE;
                m_ui->idle->show();
            }
    }

    if (prev.table != next.table) {
        if (prev.table == T_RUNNING) {
            m_ui->tableRunning->removeRow(prev.item->row());
            m_ui->running->setVisible(m_ui->tableRunning->rowCount() > 0);
        } else if (prev.table == T_QUEUED) {
            m_ui->tableQueue->removeRow(prev.item->row());
            m_ui->queue->setVisible(m_ui->tableQueue->rowCount() > 0);
        } else if (prev.table == T_IDLE) {
            m_ui->tableIdle->removeRow(prev.item->row());
            m_ui->idle->setVisible(m_ui->tableIdle->rowCount() > 0);
        }

        m_rows.insert(key, next);
        emit (isEmpty(false));
    }
}

QTableWidgetItem* QueueWidget::insertRow(TableWidget* table, Experiment* exp)
{
    const int row = table->insertRow();

    auto add = [this, table, row](TableWidget::Header header, int label) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(label));
        item->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, m_headerIdx.value(header), item);
        return item;
    };

    table->insertPlayButton(row, m_headerIdx.value(TableWidget::H_BUTTON), exp);
    add(TableWidget::H_STOPAT, exp->getStopAt());
    add(TableWidget::H_TRIALS, exp->getNumTrials());
    add(TableWidget::H_PROJID, exp->getProjId());
    return add(TableWidget::H_EXPID, exp->getId());
}
