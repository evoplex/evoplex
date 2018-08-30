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
#include "core/experimentsmgr.h"

namespace evoplex {

class RowsDelegate;

class TableWidget : public QTableWidget
{
    Q_OBJECT

    friend class RowsDelegate;

public:
    enum Header {
        H_BUTTON = 0, // always the first
        H_EXPID,
        H_SEED,
        H_STOPAT,
        H_MODEL,
        H_GRAPH,
        H_TRIALS
    };

    explicit TableWidget(QWidget* parent);
    ~TableWidget() {}

    void init(ExperimentsMgr* expMgr);

    int insertRow(Experiment* exp);

    inline const QMap<Header, QString>& headerLabels() const;

public slots:
    void removeRow(int row);

private slots:
    void onItemClicked(QTableWidgetItem* item);

private:
    const QPixmap kIcon_check;
    const QPixmap kIcon_play;
    const QPixmap kIcon_playon;
    const QPixmap kIcon_pause;
    const QPixmap kIcon_pauseon;
    const QPixmap kIcon_x;
    const QPen kPen_circleon;
    const QPen kPen_circle;

    ExperimentsMgr* m_expMgr;

    QMap<Header, QString> m_headerLabel; // map Header to column label
};

/*********************************************************/

class RowsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RowsDelegate(Experiment* exp, TableWidget* table);
    ~RowsDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:
    TableWidget* m_table;
    int m_hoveredRow;
    int m_hoveredCol;
    Status m_status;
    quint16 m_progress;

    void drawProgress(QPainter* painter, const QPointF& c) const;
};

/*********************************************************/

inline const QMap<TableWidget::Header, QString>& TableWidget::headerLabels() const
{ return m_headerLabel; }

} // evoplex
#endif // TABLEWIDGET_H
