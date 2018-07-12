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

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QPen>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QWidget>

#include "core/experiment.h"

namespace evoplex {

class RowsDelegate;

class TableWidget : public QTableWidget
{
    Q_OBJECT

    friend class RowsDelegate;

public:
    enum Header {
        H_BUTTON = 0, // always the first
        H_PROJID,
        H_EXPID,
        H_SEED,
        H_STOPAT,
        H_MODEL,
        H_GRAPH,
        H_TRIALS
    };

    explicit TableWidget(QWidget* parent);

    int insertRow(Experiment* exp);
    void insertColumns(const QList<Header> headers);

private slots:
    void onItemClicked(QTableWidgetItem* item);

private:
    const QPixmap kIcon_check;
    const QPixmap kIcon_play;
    const QPixmap kIcon_playon;
    const QPixmap kIcon_pause;
    const QPixmap kIcon_pauseon;
    const QPixmap kIcon_x;
    const QPen kPen_blue;

    QMap<Header, QString> m_headerLabel; // map Header to column label
};

/*********************************************************/
/*********************************************************/

class RowsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RowsDelegate(const Experiment* exp, TableWidget* table);
    ~RowsDelegate() {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    void onItemEntered(int row, int col);

private:
    const Experiment* m_exp;
    TableWidget* m_table;
    int m_hoveredRow;
    int m_hoveredCol;
};
}
#endif // TABLEWIDGET_H
