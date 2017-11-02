/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>

#include "graphwidget.h"
#include "ui_graphwidget.h"
#include "titlebar.h"

namespace evoplex {

GraphWidget::GraphWidget(Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui_GraphWidget)
    , m_exp(exp)
    , m_graph(nullptr)
    , m_zoomLevel(0)
    , m_nodeSizeRate(10.f)
    , m_edgeSizeRate(25.f)
    , m_nodeRadius(m_nodeSizeRate)
    , m_selectedAgent(-1)
    , m_origin(5,25)
    , m_posEntered(0,0)
{
    setWindowTitle("Graph");
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget* front = new QWidget;
    m_ui->setupUi(front);
    setWidget(front);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(trialSelected(int)), this, SLOT(setGraph(int)));
    setGraph(0);

    connect(m_ui->bZoomIn, SIGNAL(clicked(bool)), SLOT(zoomIn()));
    connect(m_ui->bZoomOut, SIGNAL(clicked(bool)), SLOT(zoomOut()));
    connect(m_ui->bShowAgents, &QPushButton::clicked, [this](bool b) { m_showAgents = b; update(); });
    connect(m_ui->bShowEdges, &QPushButton::clicked, [this](bool b) { m_showEdges = b; update(); });
    connect(m_ui->bReset, SIGNAL(clicked(bool)), SLOT(resetView()));
    m_showAgents = m_ui->bShowAgents->isChecked();
    m_showEdges = m_ui->bShowEdges->isChecked();

    Attributes attrs = m_exp->modelPlugin()->agentAttrMin;
    for (int id = 0; id < attrs.size(); ++id) {
        QLineEdit* le = new QLineEdit();
        le->setReadOnly(true);
        m_attrs.insert(id, le);
        m_ui->inspectorLayout->addRow(attrs.name(id), le);
    }
    m_ui->inspector->hide();
    connect(m_ui->bCloseInspector, SIGNAL(clicked(bool)), m_ui->inspector, SLOT(hide()));

    // update buffer when resize is done
    m_resizeTimer.setSingleShot(true);
    connect(&m_resizeTimer, &QTimer::timeout, [this](){ updateCache(); });

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);

    updateCache();
}

GraphWidget::~GraphWidget()
{
    delete m_ui;
}

void GraphWidget::setGraph(int trialId)
{
    m_graph = m_exp->graph(trialId);
    updateCache();
}

void GraphWidget::zoomIn()
{
    ++m_zoomLevel;
    m_nodeRadius = m_nodeSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GraphWidget::zoomOut()
{
    --m_zoomLevel;
    m_nodeRadius = m_nodeSizeRate * std::pow(1.25f, m_zoomLevel);
    updateCache();
}

void GraphWidget::resetView()
{
    m_origin = QPoint(5,25);
    m_zoomLevel = 0;
    m_nodeRadius = m_nodeSizeRate;
    m_selectedAgent = -1;
    m_ui->inspector->hide();
    updateCache();
}

void GraphWidget::updateCache()
{
    m_cache.clear();
    m_cache.shrink_to_fit();

    if (!m_graph)
        return;

    float edgeSizeRate = m_edgeSizeRate * std::pow(1.25f, m_zoomLevel);
    m_cache.reserve(m_graph->agents().size());
    for (Agent* agent : m_graph->agents()) {
        QPointF xy(m_origin.x() + edgeSizeRate * (1.0 + agent->x()),
                   m_origin.y() + edgeSizeRate * (1.0 + agent->y()));

        if (!rect().contains(xy.toPoint()))
            continue;

        Cache cache;
        cache.agent = agent;
        cache.xy = xy;
        cache.edges.reserve(agent->edges().size());

        for (Edge* edge : agent->edges()) {
            QPointF xy2(m_origin.x() + edgeSizeRate * (1.0 + edge->neighbour()->x()),
                        m_origin.y() + edgeSizeRate * (1.0 + edge->neighbour()->y()));
            cache.edges.emplace_back(QLineF(xy, xy2));
        }

        m_cache.emplace_back(cache);
    }
    m_cache.shrink_to_fit();
    update();
}

void GraphWidget::paintEvent(QPaintEvent* e)
{
    if (!m_graph)
        return;

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_showEdges) {
        Cache cacheSelected;
        for (Cache cache : m_cache) {
            if (m_selectedAgent == cache.agent->id()) {
                cacheSelected = cache;
            }
            for (QLineF edge : cache.edges) {
                painter.setPen(Qt::gray);
                painter.drawLine(edge);
            }
        }

        if (cacheSelected.agent) {
            for (QLineF edge : cacheSelected.edges) {
                painter.setPen(QPen(Qt::black, 3));
                painter.drawLine(edge);
            }
        }
    }

    if (m_showAgents) {
        for (Cache cache : m_cache) {
            if (m_selectedAgent == cache.agent->id()) {
                painter.setBrush(QColor(10,10,10,100));
                painter.drawEllipse(cache.xy, m_nodeRadius*1.5f, m_nodeRadius*1.5f);
            }

            if (cache.agent->attr("strategy").toInt == 0)
                painter.setBrush(Qt::blue);
            else {
                painter.setBrush(Qt::red);
            }

            painter.setPen(Qt::black);
            painter.drawEllipse(cache.xy, m_nodeRadius, m_nodeRadius);
        }
    }

    painter.end();
}

void GraphWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_posEntered = e->pos();
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_graph || e->button() != Qt::LeftButton
            || (m_ui->inspector->isVisible()
                && m_ui->inspector->geometry().contains(e->pos()))) {
        return;
    }

    m_selectedAgent = -1;
    if (e->pos() != m_posEntered) {
        m_origin += (e->pos() - m_posEntered);
        m_ui->inspector->hide();
        updateCache();
        return;
    }

    for (Cache cache : m_cache) {
        if (e->pos().x() > cache.xy.x()-m_nodeRadius
                && e->pos().x() < cache.xy.x()+m_nodeRadius
                && e->pos().y() > cache.xy.y()-m_nodeRadius
                && e->pos().y() < cache.xy.y()+m_nodeRadius) {
            m_selectedAgent = cache.agent->id();
            m_ui->agentId->setText(QString::number(m_selectedAgent));
            m_ui->neighbors->setText(QString::number(cache.edges.size()));
            for (int id = 0; id < cache.agent->attrs().size(); ++id) {
                m_attrs.value(id)->setText(cache.agent->attr(id).toQString());
            }
            m_ui->inspector->show();
            break;
        }
    }

    if (m_selectedAgent == -1) {
        m_ui->inspector->hide();
    }

    update();
}

void GraphWidget::resizeEvent(QResizeEvent* e)
{
    m_cache.clear();
    m_resizeTimer.start(500);
    QWidget::resizeEvent(e);
}

}
