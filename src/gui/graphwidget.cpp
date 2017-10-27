/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QVector>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>

#include "graphwidget.h"
#include "ui_graphwidget.h"
#include <QTimer>

namespace evoplex {

GraphWidget::GraphWidget(Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui_GraphWidget)
    //, m_graph(graph)
    , m_radius(10.f)
    , m_scale(25)
    , m_isValid(true)
    , m_origin(0,0)
{
    setWindowTitle("Graph");
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget* front = new QWidget;
    m_ui->setupUi(front);
    setWidget(front);

    m_ui->inspector->hide();

/*
    QWidget* tbar = new QWidget;
    tbar->setStyleSheet("background-color:rgb(51,51,51);");
    QHBoxLayout* l = new QHBoxLayout;
    l->addWidget(new QLabel("Trial"));
    l->addWidget(new QComboBox);
    tbar->setLayout(l);
    setTitleBarWidget(tbar);

*/
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);

QTimer *timer = new QTimer(this);
connect(timer, SIGNAL(timeout()), this, SLOT(update()));
timer->start(0);

    m_graph = exp->getGraph(0);
}

GraphWidget::~GraphWidget()
{
    delete m_ui;
}

void GraphWidget::paintEvent(QPaintEvent* e)
{
    if (!m_isValid || !m_graph)
        return;

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int agentsSize = m_graph->getAgents().size();
    const Agents& agents = m_graph->getAgents();
    for (int id = 0; id < agentsSize; ++id) {
        Agent* agent = agents.at(id);
        QPointF pos;
        pos.setX(m_origin.x() + m_scale + (m_scale * agent->getX()));
        pos.setY(m_origin.y() + m_scale + (m_scale * agent->getY()));

        if (!e->region().contains(pos.toPoint()))
            continue;

        if (m_posClicked.x() > pos.x()-m_radius && m_posClicked.x() < pos.x()+m_radius
                && m_posClicked.y() > pos.y()-m_radius && m_posClicked.y() < pos.y()+m_radius) {
            painter.setBrush(QColor(10,10,10,100));
            painter.drawEllipse(pos, m_radius*1.5f, m_radius*1.5f);
        }

        if (agent->attribute("strategy").toInt == 0)
            painter.setBrush(Qt::blue);
        else {
            painter.setBrush(Qt::red);
        }

        painter.setPen(Qt::black);
        painter.drawEllipse(pos, m_radius, m_radius);
    }

    painter.end();
}

void GraphWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton)
        return;
    m_posClicked = e->pos();
    update();
}

void GraphWidget::mouseMoveEvent(QMouseEvent* e)
{
    m_origin = e->pos() - m_posClicked;
    qDebug() << m_origin << e->pos() << m_posClicked;
}
}
