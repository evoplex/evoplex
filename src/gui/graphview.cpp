/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QPainter>

#include "graphview.h"
#include "ui_graphwidget.h"
#include "ui_graphsettings.h"

namespace evoplex
{

GraphView::GraphView(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent)
    : GraphWidget(mainGUI, exp, parent)
    , m_edgeSizeRate(25.f)
{
    setWindowTitle("Graph");

    m_edgeAttr = m_settingsDlg->edgeAttr();
    m_edgeCMap = m_settingsDlg->edgeCMap();
    connect(m_settingsDlg, &GraphSettings::edgeAttrUpdated, [this](int idx) { m_edgeAttr = idx; });
    connect(m_settingsDlg, &GraphSettings::edgeCMapUpdated, [this](ColorMap* cmap) {
        delete m_edgeCMap;
        m_edgeCMap = cmap;
        update();
    });

    m_showAgents = m_ui->bShowAgents->isChecked();
    m_showEdges = m_ui->bShowEdges->isChecked();
    connect(m_ui->bShowAgents, &QPushButton::clicked, [this](bool b) { m_showAgents = b; update(); });
    connect(m_ui->bShowEdges, &QPushButton::clicked, [this](bool b) { m_showEdges = b; update(); });

    setTrial(0); // init at trial 0
}

int GraphView::refreshCache()
{
    if (paintingActive()) {
        return Scheduled;
    }
    Utils::deleteAndShrink(m_cache);
    if (!m_model) {
        return Ready;
    }

    const Agents agents = m_model->graph()->agents();
    float edgeSizeRate = m_edgeSizeRate * std::pow(1.25f, m_zoomLevel);
    m_cache.reserve(agents.size());

    for (Agent* agent : agents) {
        QPointF xy(m_origin.x() + edgeSizeRate * (1.0 + agent->x()),
                   m_origin.y() + edgeSizeRate * (1.0 + agent->y()));

        if (!rect().contains(xy.toPoint()))
            continue;

        Cache cache;
        cache.agent = agent;
        cache.xy = xy;
        cache.edges.reserve(agent->edges().size());

        for (const Edge* edge : agent->edges()) {
            QPointF xy2(m_origin.x() + edgeSizeRate * (1.0 + edge->neighbour()->x()),
                        m_origin.y() + edgeSizeRate * (1.0 + edge->neighbour()->y()));
            cache.edges.emplace_back(QLineF(xy, xy2));
        }

        m_cache.emplace_back(cache);
    }
    m_cache.shrink_to_fit();

    return Ready;
}

void GraphView::paintEvent(QPaintEvent*)
{
    if (m_cacheStatus != Ready) {
        return;
    }

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_showEdges) {
        Cache cacheSelected;
        for (const Cache& cache : m_cache) {
            if (m_selectedAgent == cache.agent->id()) {
                cacheSelected = cache;
            }
            for (const QLineF& edge : cache.edges) {
                painter.setPen(Qt::gray);
                painter.drawLine(edge);
            }
        }

        if (cacheSelected.agent) {
            for (const QLineF& edge : cacheSelected.edges) {
                painter.setPen(QPen(Qt::black, 3));
                painter.drawLine(edge);
            }
        }
    }

    if (m_showAgents) {
        for (const Cache& cache : m_cache) {
            if (m_selectedAgent == cache.agent->id()) {
                painter.setBrush(QColor(10,10,10,100));
                painter.drawEllipse(cache.xy, m_nodeRadius*1.5f, m_nodeRadius*1.5f);
            }

            const Value& value = cache.agent->attr(m_agentAttr);
            painter.setBrush(m_agentCMap->colorFromValue(value));
            painter.setPen(Qt::black);
            painter.drawEllipse(cache.xy, m_nodeRadius, m_nodeRadius);
        }
    }

    painter.end();
}

const Agent* GraphView::selectAgent(const QPoint& pos) const
{
    if (m_cacheStatus == Ready) {
        for (const Cache& cache : m_cache) {
            if (pos.x() > cache.xy.x()-m_nodeRadius
                    && pos.x() < cache.xy.x()+m_nodeRadius
                    && pos.y() > cache.xy.y()-m_nodeRadius
                    && pos.y() < cache.xy.y()+m_nodeRadius) {
                return cache.agent;
            }
        }
    }
    return nullptr;
}

} // evoplex
