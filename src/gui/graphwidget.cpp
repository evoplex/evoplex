/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QtDebug>
#include <QtMath>
#include <QVector>

#include "graphwidget.h"
#include <QTimer>

namespace evoplex {

GraphWidget::GraphWidget(Experiment* experiment, QWidget* parent)
    : QDockWidget(parent)
    //, m_graph(graph)
    , m_radius(10.f)
    , m_scale(25)
    , m_isValid(true)
{
    this->setWindowTitle("Graph");

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);

QTimer *timer = new QTimer(this);
connect(timer, SIGNAL(timeout()), this, SLOT(update()));
timer->start(0);

    m_graph = experiment->getGraph(0);
    //m_graph->resetCoordinates();
}

GraphWidget::~GraphWidget()
{
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
        const Agent* agent = agents.at(id);
        QPointF pos;
        pos.setX(m_scale + (m_scale * agent->getX()));
        pos.setY(m_scale + (m_scale * agent->getY()));

        if (m_posClicked.x() > pos.x()-m_radius && m_posClicked.x() < pos.x()+m_radius
                && m_posClicked.y() > pos.y()-m_radius && m_posClicked.y() < pos.y()+m_radius) {
            painter.setBrush(Qt::yellow);
        } else {
            if (agent->attribute("strategy").toInt == 0)
                painter.setBrush(Qt::blue);
            else {
                painter.setBrush(Qt::red);
            }
        }
        painter.setPen(QColor("black"));
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
}
}
