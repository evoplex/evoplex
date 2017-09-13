/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include <QScrollArea>

#include "ui_queuewidget.h"
#include "core/experimentsmgr.h"
#include "gui/tablewidget.h"

class QueueWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit QueueWidget(ExperimentsMgr* expMgr, QWidget* parent = nullptr);

signals:
    void isEmpty(bool empty);

private slots:
    void slotStatusChanged(Experiment* exp);

private:
    enum Table {
        T_RUNNING,
        T_QUEUED,
        T_IDLE,
        T_INVALID
    };

    struct Row {
        int row = -1;
        Table table = T_INVALID;
    };

    Ui_QueueWidget* m_ui;
    ExperimentsMgr* m_expMgr;

    QHash<QString, Row> m_rows; // map 'projId.expId' to the row
    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    int insertRow(TableWidget *table, Experiment* exp);
};

#endif // QUEUEWIDGET_H
