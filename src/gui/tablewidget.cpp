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
    , kIcon_restart(QPixmap(":/icons/restart.svg").scaledToWidth(18, Qt::SmoothTransformation))
    , kIcon_x(QPixmap(":/icons/x.svg").scaledToWidth(14, Qt::SmoothTransformation))
{
    this->setMouseTracking(true);

    this->setStyleSheet("QTableView { background-color:transparent; selection-background-color: rgb(51,51,51); }"
                        "QTableView::item { border-bottom: 1px solid rgb(40,40,40); color: white; }"
                        "QTableView::item:hover { background-color: rgb(40,40,40); }");

    this->horizontalHeader()->setStyleSheet(
                "QHeaderView { background-color: transparent; }"
                "QHeaderView::section {\
                    background-color: transparent; \
                    color: rgb(145,145,145);\
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

    this->horizontalHeader()->setHighlightSections(false);
    this->horizontalHeader()->setDefaultSectionSize(70);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->verticalHeader()->setVisible(false);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->setDefaultSectionSize(40);

    this->setShowGrid(false);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setItemDelegate(new RowsDelegate(this));

    // setup the context menu
//    m_contextMenu = new ContextMenuTable(m_project, m_tableExps);
//    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(m_tableExps, SIGNAL(customContextMenuRequested(QPoint)),
//            this, SLOT(slotContextMenu(QPoint)));
//    connect(m_contextMenu, SIGNAL(openView(int)), this, SLOT(slotOpenView(int)));
}

void TableWidget::insertColumns(const QList<Header> headers)
{
    QStringList labels;
    foreach (Header h, headers) {
        labels << m_headerLabel.value(h);
    }
    this->setColumnCount(labels.size());
    this->setHorizontalHeaderLabels(labels);
}

void TableWidget::insertPlayButton(int row, int col, Experiment* exp)
{
    // the row number might change (move), so lets insert an empty
    // item just to make easier for the button to know where it is
    QTableWidgetItem* item = new QTableWidgetItem("");
    this->setItem(row, col, item);

    this->setCellWidget(row, col, new PlayButton(item, exp, this));
    this->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Fixed);
    this->horizontalHeader()->setDefaultSectionSize(60);
}

/*********************************************************/
/*********************************************************/

PlayButton::PlayButton(QTableWidgetItem* item, Experiment* exp, TableWidget* parent)
    : QWidget(parent)
    , m_table(parent)
    , m_exp(exp)
    , m_item(item)
    , m_btnHovered(false)
    , m_rowHovered(false)
    , m_penBlue(QPen(QBrush(QColor(66,133,244)), 3))
{
    connect(parent, &QTableWidget::viewportEntered, [this](){ m_rowHovered=false; });
    connect(parent, SIGNAL(cellEntered(int,int)), this, SLOT(onItemEntered(int,int)));
}

void PlayButton::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
        m_exp->toggle();
}

void PlayButton::enterEvent(QEvent*)
{
    m_btnHovered = true;
    if (m_item) emit (m_table->cellEntered(m_item->row(), 0));
}

void PlayButton::leaveEvent(QEvent*)
{
    m_btnHovered = false;
}

void PlayButton::onItemEntered(int row, int col)
{
    Q_UNUSED(col);
    try {
        m_rowHovered = m_item->row() == row;
    } catch (...) {
        m_rowHovered = false;
    }
}

void PlayButton::paintEvent(QPaintEvent* e)
{
    if (!exp)
        return;

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint center = rect().center();
    Experiment::Status status = m_exp->getExpStatus();
    if (status == Experiment::READY) {
        if (m_btnHovered) { //play (only when hovered)
            painter.drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_playon);
        } else if (m_rowHovered) {
            painter.drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_play);
        }
        // show progress
        if (m_exp->getProgress() > 0) {
            painter.setPen(m_penBlue);
            painter.drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_exp->getProgress()*16);
        }
    } else if (status == Experiment::RUNNING) {
        if (m_btnHovered || m_rowHovered) { // pause (always show)
            painter.drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pauseon);
        } else {
            painter.drawPixmap(center.x()-14, center.y()-14, m_table->kIcon_pause);
        }
        // show progress
        if (m_exp->getProgress() > 0) {
            painter.setPen(m_penBlue);
            painter.drawArc(center.x()-14, center.y()-14, 28, 28, 90*16, -m_exp->getProgress()*16);
        }
    } else if (status == Experiment::FINISHED) {
        if (m_btnHovered || m_rowHovered) { // restart (when hovered)
            painter.drawPixmap(center.x()-9, center.y()-9, m_table->kIcon_restart);
        } else { // check (always)
            painter.drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_check);
        }
    } else {
        painter.drawPixmap(center.x()-7, center.y()-7, m_table->kIcon_x);
    }

    painter.end();
}

/*********************************************************/
/*********************************************************/

RowsDelegate::RowsDelegate(QTableWidget* tableWidget)
    : QStyledItemDelegate(tableWidget)
    , m_tableWdt(tableWidget)
    , m_hoveredRow(-1)
{
    connect(m_tableWdt, &QTableWidget::viewportEntered, [this](){ m_hoveredRow=-1; });
    connect(m_tableWdt, SIGNAL(cellEntered(int,int)), this, SLOT(onItemEntered(int,int)));
}

void RowsDelegate::onItemEntered(int row, int col)
{
    Q_UNUSED(col);
    m_hoveredRow = row;
    m_tableWdt->viewport()->update();
}

void RowsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;

    // removes the border around the selected cell
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    // highlight the entire row
    QPoint pos = m_tableWdt->viewport()->mapFromGlobal(QCursor::pos());
    QSize sz = m_tableWdt->viewport()->size();
    if (index.row() == m_hoveredRow
            && pos.x() >= 0 && pos.x() <= sz.width()
            && pos.y() >= 0 && pos.y() <= sz.height())
        opt.state |= QStyle::State_MouseOver;

    QStyledItemDelegate::paint(painter, opt, index);
}
}
