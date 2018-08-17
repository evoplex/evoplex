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

#include <QPainter>

#include "core/trial.h"

#include "graphview.h"
#include "ui_basegraphgl.h"
#include "ui_graphsettings.h"
#include "utils.h"

namespace evoplex
{

GraphView::GraphView(ExperimentPtr exp, GraphWidget* parent)
    : BaseGraphGL(exp, parent),
      m_edgeSizeRate(25.)
{
    setWindowTitle("Graph");

    auto s = m_graphWidget->settingsDlg();
    m_edgeAttr = s->edgeAttr();
    m_edgeCMap = s->edgeCMap();
    connect(s, &GraphSettings::edgeAttrUpdated, [this](int idx) { m_edgeAttr = idx; });
    connect(s, &GraphSettings::edgeCMapUpdated, [this](ColorMap* cmap) {
        delete m_edgeCMap;
        m_edgeCMap = cmap;
        update();
    });

    m_showNodes = m_ui->bShowNodes->isChecked();
    m_showEdges = m_ui->bShowEdges->isChecked();
    connect(m_ui->bShowNodes, &QPushButton::clicked, [this](bool b) { m_showNodes = b; update(); });
    connect(m_ui->bShowEdges, &QPushButton::clicked, [this](bool b) { m_showEdges = b; update(); });

    setTrial(0); // init at trial 0
}

CacheStatus GraphView::refreshCache()
{
    if (paintingActive()) {
        return CacheStatus::Scheduled;
    }
    Utils::deleteAndShrink(m_cache);
    if (!m_trial || !m_trial->graph()) {
        return CacheStatus::Ready;
    }

    qreal edgeSizeRate = m_edgeSizeRate * std::pow(1.25f, m_zoomLevel);
    m_cache.reserve(m_trial->graph()->nodes().size());

    for (auto const& np : m_trial->graph()->nodes()) {
        QPointF xy(m_origin.x() + edgeSizeRate * (1. + np.second.x()),
                   m_origin.y() + edgeSizeRate * (1. + np.second.y()));

        if (!rect().contains(xy.toPoint())) {
            continue;
        }

        Cache cache;
        cache.node = np.second;
        cache.xy = xy;
        cache.edges.reserve(np.second.outEdges().size());

        for (auto const& ep : np.second.outEdges()) {
            QPointF xy2(m_origin.x() + edgeSizeRate * (1.0 + ep.second.neighbour().x()),
                        m_origin.y() + edgeSizeRate * (1.0 + ep.second.neighbour().y()));
            cache.edges.emplace_back(QLineF(xy, xy2));
        }

        m_cache.emplace_back(cache);
    }
    m_cache.shrink_to_fit();

    return CacheStatus::Ready;
}

void GraphView::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), m_background);

    if (m_cacheStatus != CacheStatus::Ready) {
        painter.end();
        return;
    }

    if (m_showEdges) {
        Cache cacheSelected;
        for (const Cache& cache : m_cache) {
            if (m_selectedNode == cache.node.id()) {
                cacheSelected = cache;
            }
            for (const QLineF& edge : cache.edges) {
                painter.setPen(Qt::gray);
                painter.drawLine(edge);
            }
        }

        if (!cacheSelected.node.isNull()) {
            for (const QLineF& edge : cacheSelected.edges) {
                painter.setPen(QPen(Qt::black, 3));
                painter.drawLine(edge);
            }
        }
    }

    if (m_showNodes) {
        for (const Cache& cache : m_cache) {
            if (m_selectedNode == cache.node.id()) {
                painter.setBrush(QColor(10,10,10,100));
                painter.drawEllipse(cache.xy, m_nodeRadius*1.5, m_nodeRadius*1.5);
            }

            if (m_nodeAttr >= 0) {
                const Value& value = cache.node.attr(m_nodeAttr);
                painter.setBrush(m_nodeCMap->colorFromValue(value));
            } else {
                painter.setBrush(m_nodeCMap->colors().front());
            }
            painter.setPen(Qt::black);
            painter.drawEllipse(cache.xy, m_nodeRadius, m_nodeRadius);
        }
    }

    painter.end();
}

Node GraphView::selectNode(const QPoint& pos) const
{
    if (m_cacheStatus == CacheStatus::Ready) {
        for (const Cache& cache : m_cache) {
            if (pos.x() > cache.xy.x()-m_nodeRadius
                    && pos.x() < cache.xy.x()+m_nodeRadius
                    && pos.y() > cache.xy.y()-m_nodeRadius
                    && pos.y() < cache.xy.y()+m_nodeRadius) {
                return cache.node;
            }
        }
    }
    return Node();
}

} // evoplex
