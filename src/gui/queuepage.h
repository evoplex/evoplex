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

#ifndef QUEUEPAGE_H
#define QUEUEPAGE_H

#include <QScrollArea>

#include "maingui.h"
#include "tablewidget.h"

class Ui_QueuePage;

namespace evoplex {

class QueuePage : public QScrollArea
{
    Q_OBJECT

public:
    explicit QueuePage(MainGUI* mainGUI);

signals:
    void isEmpty(bool empty);

private slots:
    void slotStatusChanged(Experiment* exp);

private:
    typedef std::pair<int, int> rowKey; // <projId, expId>

    struct Row {
        QTableWidgetItem* item = nullptr; // hold an item just to get access to the current row number
        TableWidget* table = nullptr;
        QWidget* section = nullptr;
    };

    Ui_QueuePage* m_ui;

    QHash<rowKey, Row> m_rows; // map 'projId.expId' to the Row
    QMap<TableWidget::Header, int> m_headerIdx; // map Header to column index

    QTableWidgetItem* insertRow(TableWidget* table, Experiment* exp);
    void moveRow(TableWidget* prevTable, int preRow, TableWidget* nextTable, Experiment* exp);
    void removeRow(const Row& key);
};
}
#endif // QUEUEPAGE_H
