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

#include <QHeaderView>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QDebug>

#include "tablewidget.h"

namespace evoplex {

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
    , kIcon_check(QPixmap(":/icons/check.svg").scaledToWidth(14, Qt::SmoothTransformation))
    , kIcon_play(QPixmap(":/icons/play-circle.svg").scaledToWidth(28, Qt::SmoothTransformation))
    , kIcon_playon(QPixmap(":/icons/play-circle-on.svg").scaledToWidth(28, Qt::SmoothTransformation))
    , kIcon_pause(QPixmap(":/icons/pause-circle.svg").scaledToWidth(28, Qt::SmoothTransformation))
    , kIcon_pauseon(QPixmap(":/icons/pause-circle-on.svg").scaledToWidth(28, Qt::SmoothTransformation))
    , kIcon_x(QPixmap(":/icons/x.svg").scaledToWidth(14, Qt::SmoothTransformation))
    , kPen_blue(QPen(QBrush(QColor(66,133,244)), 3))
{
    setMouseTracking(true);

    setStyleSheet(
        "QTableView { background-color:transparent; selection-background-color: rgb(51,51,51); }"
        "QTableView::item { border-bottom: 1px solid rgb(40,40,40); color: white; }"
        "QTableView::item:hover { background-color: rgb(40,40,40); }"
    );

    horizontalHeader()->setStyleSheet(
        "QHeaderView { background-color: transparent; }"
        "QHeaderView::section {\
            background-color: transparent; \
            color: rgb(145,145,145); \
            padding-left: 4px;\
            border: 0px;\
            border-bottom: 1px solid rgb(40,40,40);}"
    );

    m_headerLabel.insert(H_BUTTON, "");
    m_headerLabel.insert(H_PROJID, "Project");
    m_headerLabel.insert(H_EXPID, "#");
    m_headerLabel.insert(H_SEED, "Seed");
    m_headerLabel.insert(H_STOPAT, "Stop at");
    m_headerLabel.insert(H_MODEL, "Model");
    m_headerLabel.insert(H_GRAPH, "Graph");
    m_headerLabel.insert(H_TRIALS, "Trials");

    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(40);

    setShowGrid(false);
    setSortingEnabled(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, SIGNAL(itemClicked(QTableWidgetItem*)),
            SLOT(onItemClicked(QTableWidgetItem*)));

    // setup the context menu
//    m_contextMenu = new ContextMenuTable(m_project, m_tableExps);
//    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(m_tableExps, SIGNAL(customContextMenuRequested(QPoint)),
//            this, SLOT(slotContextMenu(QPoint)));
//    connect(m_contextMenu, SIGNAL(openView(int)), this, SLOT(slotOpenView(int)));
}

void TableWidget::init(ExperimentsMgr* expMgr)
{
    m_expMgr = expMgr;
    connect(m_expMgr, SIGNAL(progressUpdated()), viewport(), SLOT(update()));
}

void TableWidget::insertColumns(const QList<Header> headers)
{
    Q_ASSERT_X(headers.at(0) == H_BUTTON, "TableWidget::insertColumns",
             "the toggle button MUST be in the first column");

    QStringList labels;
    labels << m_headerLabel.value(H_BUTTON);
    for (int i = 1; i < headers.size(); ++i) {
        Header h = headers.at(i);
        Q_ASSERT_X(h != H_BUTTON, "TableWidget::insertColumns",
                   "we must have only one toggle button in a row");
        labels << m_headerLabel.value(h);
    }
    setColumnCount(labels.size());
    setHorizontalHeaderLabels(labels);
}

int TableWidget::insertRow(Experiment* exp)
{
    Q_ASSERT_X(m_expMgr, "TableWidget", "table must be initialized first");

    int row = rowCount();
    QTableWidget::insertRow(row);

    // to make the toggle button work properly,
    // we need to attach the Experiment* to it
    QTableWidgetItem* item = new QTableWidgetItem("");
    item->setData(Qt::UserRole, QVariant::fromValue(exp));
    setItem(row, 0, item); // always in the first column

    horizontalHeader()->setDefaultSectionSize(60);
    horizontalHeader()->setSectionResizeMode(H_BUTTON, QHeaderView::Fixed);

    setItemDelegateForRow(row, new RowsDelegate(exp, this));

    return row;
}

void TableWidget::onItemClicked(QTableWidgetItem* item)
{
    if (!item || item->column() != 0)
        return; // it's not the button

    Experiment* exp = item->data(Qt::UserRole).value<Experiment*>();
    if (exp)
        exp->toggle();
}

void TableWidget::removeRow(int row)
{
    // Item delegates are not updated automatically.
    // For example, when we remove the row 2, the row 3 will become the row 2
    // but will continue to use the delegate of the removed row 2. So, we need
    // to reassign all delegates below the removed row.
    const int lastRow = rowCount() - 1;
    for (int r = row; r < lastRow; ++r) {
        setItemDelegateForRow(r, itemDelegateForRow(r+1));
    }
    setItemDelegateForRow(lastRow, nullptr);
    model()->removeRow(row);
}

/*********************************************************/
/*********************************************************/

RowsDelegate::RowsDelegate(Experiment* exp, TableWidget* table)
    : QStyledItemDelegate(table),
      m_table(table),
      m_hoveredRow(-1),
      m_hoveredCol(-1),
      m_status(exp->expStatus()),
      m_progress(exp->progress())
{
    connect(m_table, &QTableWidget::viewportEntered,
        [this]() { m_hoveredRow = -1; m_hoveredCol = -1; });

    connect(m_table, &QTableWidget::cellEntered,
        [this](int row, int col) { m_hoveredRow = row; m_hoveredCol = col; });

    connect(exp, &Experiment::statusChanged,
        [this](Status s) { m_status = s; });

    connect(exp, &Experiment::progressUpdated,
        [this](quint16 p) { m_progress = p; });
}

RowsDelegate::~RowsDelegate()
{
}

void RowsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;

    //
    // removes the border around the selected cell
    //
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    //
    // highlight the entire row
    //
    bool rowIsHovered = false;
    QPoint pos = m_table->viewport()->mapFromGlobal(QCursor::pos());
    QSize sz = m_table->viewport()->size();
    if (index.row() == m_hoveredRow
            && pos.x() >= 0 && pos.x() <= sz.width()
            && pos.y() >= 0 && pos.y() <= sz.height()) {
        opt.state |= QStyle::State_MouseOver;
        rowIsHovered = true;
    }
    QStyledItemDelegate::paint(painter, opt, index);

    if (index.column() != 0) {
        return;
    }

    //
    // draw the toggle button in the first column
    //
    bool btnIsHovered = rowIsHovered && m_hoveredCol == index.column();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QPoint center = opt.rect.center();
    if (m_status == Status::Paused || m_status == Status::Disabled) {
        if (btnIsHovered) { //play (only when hovered)
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_playon);
        } else if (rowIsHovered) {
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_play);
        }
        // show progress
        if (m_progress > 0) {
            painter->setPen(m_table->kPen_blue);
            painter->drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_progress*16);
        }
    } else if (m_status == Status::Running || m_status == Status::Queued) {
        if (btnIsHovered || rowIsHovered) { // pause (always show)
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pauseon);
        } else {
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pause);
        }
        // show progress
        if (m_progress > 0) {
            painter->setPen(m_table->kPen_blue);
            painter->drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_progress*16);
        }
    } else if (m_status == Status::Finished) { // check (always)
        painter->drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_check);
    } else {
        painter->drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_x);
    }
    painter->restore();
}
}
