/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QHeaderView>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

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
    m_headerLabel.insert(H_AGENTS, "Agents");
    m_headerLabel.insert(H_MODEL, "Model");
    m_headerLabel.insert(H_GRAPH, "Graph");
    m_headerLabel.insert(H_TRIALS, "Trials");

    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setDefaultSectionSize(70);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(40);

    setShowGrid(false);
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

void TableWidget::insertColumns(const QList<Header> headers)
{
    // make sure the toggle button is in the first column
    Q_ASSERT(headers.at(0) == H_BUTTON);
    QStringList labels;
    labels << m_headerLabel.value(H_BUTTON);
    for (int i = 1; i < headers.size(); ++i) {
        Header h = headers.at(i);
        Q_ASSERT(h != H_BUTTON); // make sure we have only one toggle button
        labels << m_headerLabel.value(h);
    }
    setColumnCount(labels.size());
    setHorizontalHeaderLabels(labels);
}

int TableWidget::insertRow(Experiment* exp)
{
    int row = rowCount();
    QTableWidget::insertRow(row);

    // to make the toggle button work properly,
    // we need to attach the Experiment* to it
    QTableWidgetItem* item = new QTableWidgetItem("");
    item->setData(Qt::UserRole, QVariant::fromValue(exp));
    setItem(row, 0, item); // always in the first column
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->setDefaultSectionSize(60);

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

/*********************************************************/
/*********************************************************/

RowsDelegate::RowsDelegate(Experiment* exp, TableWidget* table)
    : QStyledItemDelegate(table)
    , m_table(table)
    , m_exp(exp)
    , m_hoveredRow(-1)
    , m_hoveredCol(-1)
{
    connect(m_table, &QTableWidget::viewportEntered, [this](){ m_hoveredRow=-1; m_hoveredCol=-1; });
    connect(m_table, SIGNAL(cellEntered(int,int)), SLOT(onItemEntered(int,int)));
}

void RowsDelegate::onItemEntered(int row, int col)
{
    m_hoveredRow = row;
    m_hoveredCol = col;
    m_table->viewport()->update();
}

void RowsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    if (m_table->rowCount() <= 0)
        return;

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

    if (index.column() != 0)
        return;

    //
    // draw the toggle button in the first column
    //
    bool btnIsHovered = rowIsHovered && m_hoveredCol == index.column();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QPoint center = opt.rect.center();
    Experiment::Status status = m_exp->getExpStatus();
    if (status == Experiment::READY) {
        if (btnIsHovered) { //play (only when hovered)
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_playon);
        } else if (rowIsHovered) {
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_play);
        }
        // show progress
        if (m_exp->getProgress() > 0) {
            painter->setPen(m_table->kPen_blue);
            painter->drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_exp->getProgress()*16);
        }
    } else if (status == Experiment::RUNNING || status == Experiment::QUEUED) {
        if (btnIsHovered || rowIsHovered) { // pause (always show)
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pauseon);
        } else {
            painter->drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pause);
        }
        // show progress
        if (m_exp->getProgress() > 0) {
            painter->setPen(m_table->kPen_blue);
            painter->drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_exp->getProgress()*16);
        }
    } else if (status == Experiment::FINISHED) { // check (always)
        painter->drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_check);
    } else {
        painter->drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_x);
    }
    painter->restore();
}
}
