/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QMouseEvent>
#include <QPen>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QWidget>

#include "core/experiment.h"

class PlayButton;
class RowsDelegate;

class TableWidget : public QTableWidget
{
    Q_OBJECT

    friend class PlayButton;

public:
    enum Header {
        H_BUTTON,
        H_PROJID,
        H_EXPID,
        H_SEED,
        H_STOPAT,
        H_AGENTS,
        H_MODEL,
        H_GRAPH,
        H_TRIALS
    };

    explicit TableWidget(QWidget* parent = 0);

    inline int insertRow() { int r = rowCount(); QTableWidget::insertRow(r); return r; }
    void insertColumns(const QList<Header> headers);
    void insertPlayButton(int row, int col, Experiment* exp);

private:
    const QPixmap kIcon_check;
    const QPixmap kIcon_play;
    const QPixmap kIcon_playon;
    const QPixmap kIcon_pause;
    const QPixmap kIcon_pauseon;
    const QPixmap kIcon_restart;
    const QPixmap kIcon_x;

    QMap<Header, QString> m_headerLabel; // map Header to column label
};

/*********************************************************/
/*********************************************************/

class PlayButton : public QWidget
{
    Q_OBJECT

public:
    explicit PlayButton(QTableWidgetItem* item, Experiment* exp, TableWidget *parent);
    ~PlayButton() {}

signals:
    void cellEntered(int row, int col);

private slots:
    void onItemEntered(int row, int col);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void mousePressEvent(QMouseEvent* e);

private:
    TableWidget* m_table;
    Experiment* m_exp;
    QTableWidgetItem* m_item;
    bool m_btnHovered;
    bool m_rowHovered;

    const QPen m_penBlue;
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

#endif // TABLEWIDGET_H
