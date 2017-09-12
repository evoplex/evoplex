#include <QPainter>
#include <QPaintEvent>

#include "gui/tablewidget.h"

TableWidget::TableWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_TableWidget)
{
    m_ui->setupUi(this);

    m_ui->table->setStyleSheet("QTableView { background-color:transparent; selection-background-color: rgb(51,51,51); }"
                               "QTableView::item { border-bottom: 1px solid rgb(40,40,40); color: white; }"
                               "QTableView::item:hover { background-color: rgb(40,40,40); }");

    m_ui->table->horizontalHeader()->setStyleSheet(
                "QHeaderView { background-color: rgb(24,24,24); }"
                "QHeaderView::section {\
                    background-color: rgb(24,24,24); \
                    color: rgb(145,145,145);\
                    padding-left: 4px;\
                    border: 0px;\
                    border-bottom: 1px solid rgb(40,40,40);}"
                );

    m_headerLabel.insert(H_BUTTON, "");
    m_headerLabel.insert(H_EXPID, "#");
    m_headerLabel.insert(H_SEED, "Seed");
    m_headerLabel.insert(H_STOPAT, "Stop at");
    m_headerLabel.insert(H_AGENTS, "Agents");
    m_headerLabel.insert(H_MODEL, "Model");
    m_headerLabel.insert(H_GRAPH, "Graph");
    m_headerLabel.insert(H_TRIALS, "Trials");

    m_ui->table->horizontalHeader()->setHighlightSections(false);
    m_ui->table->horizontalHeader()->setDefaultSectionSize(70);
    m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_ui->table->verticalHeader()->setVisible(false);
    m_ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_ui->table->verticalHeader()->setDefaultSectionSize(40);

    m_ui->table->setShowGrid(false);
    m_ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->table->setItemDelegate(new RowsDelegate(m_ui->table));

    connect(m_ui->table, SIGNAL(itemClicked(QTableWidgetItem*)),
            this, SIGNAL(itemClicked(QTableWidgetItem*)));
    connect(m_ui->table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)));
}

TableWidget::~TableWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void TableWidget::insertColumns(const QList<int> headers)
{
    QStringList labels;
    foreach (int h, headers) {
        labels << m_headerLabel.value((Header) h);
    }
    m_ui->table->setColumnCount(labels.size());
    m_ui->table->setHorizontalHeaderLabels(labels);
}

/*********************************************************/
/*********************************************************/

PlayButton::PlayButton(int row, Experiment* exp, QTableWidget* parent)
    : QWidget(parent)
    , m_exp(exp)
    , m_row(row)
    , m_btnHovered(false)
    , m_rowHovered(false)
{
    connect(this, SIGNAL(cellEntered(int,int)), parent, SIGNAL(cellEntered(int,int)));
    connect(parent, SIGNAL(cellEntered(int,int)), this, SLOT(onItemEntered(int,int)));
}

void PlayButton::onItemEntered(int row, int col)
{
    Q_UNUSED(col);
    m_rowHovered = m_row == row;
}

void PlayButton::paintEvent(QPaintEvent* e)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //painter.setBrush(QBrush);
    //painter.setPen(QColor("blue"));

    int xCenter = e->rect().center().x();

    Experiment::Status status = *m_exp->getExpStatusP();
    if (status == Experiment::READY) {
        if (m_btnHovered || m_rowHovered) { //play (only when hovered)
            float x = xCenter + 16.5;
            float r = 5.0;
            QPointF play[3] = {
                QPointF(x-r, 12.), // top
                QPointF(x-r, 27.5), // bottom
                QPointF(x+r, 20.5)  // right
            };
            painter.drawPolygon(play, 3);
            painter.drawArc(xCenter, 5, 29, 29, 0, 360*16);
        }
    } else if (status == Experiment::RUNNING) {
        //pause (always)
        //show progressbar (ao redor com percent na tooltip ou pequeno no topo)
    } else if (status == Experiment::FINISHED) {
        //check (always)
        //restart (when hovered)
    } else {
        //red cross (always)
    }

    if (m_btnHovered) { // button hovered
        // bright
    } else if (m_rowHovered) { // row hovered
        // normal
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
    m_tableWdt->setMouseTracking(true);
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
