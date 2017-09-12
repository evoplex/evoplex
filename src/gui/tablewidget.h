#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QMouseEvent>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QWidget>

#include "ui_tablewidget.h"
#include "core/experiment.h"

class PlayButton : public QWidget
{
    Q_OBJECT

public:
    explicit PlayButton(int row, Experiment* exp, QTableWidget* parent);

signals:
    void cellEntered(int row, int col);

private slots:
    void onItemEntered(int row, int col);

protected:
    virtual void paintEvent(QPaintEvent* e);
    inline void enterEvent(QEvent*) { m_btnHovered = true; emit (cellEntered(m_row, 0)); }
    inline void leaveEvent(QEvent*) { m_btnHovered = false; }
    inline void mousePressEvent(QMouseEvent* e) { if(e->button() == Qt::LeftButton) m_exp->toggle(); }

private:
    Experiment* m_exp;
    int m_row;
    bool m_btnHovered;
    bool m_rowHovered;
};

/*********************************************************/
/*********************************************************/

class RowsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    RowsDelegate(QTableWidget* tableWidget);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    void onItemEntered(int row, int col);

private:
    Project* m_project;
    QTableWidget *m_tableWdt;
    int m_hoveredRow;
};

/*********************************************************/
/*********************************************************/

class TableWidget : public QWidget
{
    Q_OBJECT

public:
    enum Header {
        H_BUTTON,
        H_EXPID,
        H_SEED,
        H_STOPAT,
        H_AGENTS,
        H_MODEL,
        H_GRAPH,
        H_TRIALS
    };

    explicit TableWidget(QWidget *parent = 0);
    ~TableWidget();

    inline int rowCount() { m_ui->table->rowCount(); }
    inline void insertRow(int row) { m_ui->table->insertRow(row); }
    inline void setItem(int row, int col, QTableWidgetItem* item) { m_ui->table->setItem(row, col, item); }
    void insertColumns(const QList<int> headers);
    inline QVariant data(int row, int col) { m_ui->table->item(row, col)->data(Qt::DisplayRole); }

    inline void stretchColumn(int col) {
        m_ui->table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }

    inline void insertPlayButton(int row, int col, Experiment* exp) {
        m_ui->table->setCellWidget(row, col, new PlayButton(row, exp, m_ui->table));
        m_ui->table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Fixed);
        m_ui->table->horizontalHeader()->setDefaultSectionSize(60);
    }

signals:
    void itemClicked(QTableWidgetItem*);
    void itemDoubleClicked(QTableWidgetItem*);

private:
    Ui_TableWidget* m_ui;
    QMap<Header, QString> m_headerLabel; // map Header to column label
};

#endif // TABLEWIDGET_H
