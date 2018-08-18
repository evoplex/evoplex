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

namespace evoplex {

GraphView::GraphView(ColorMapMgr* cMgr, ExperimentPtr exp, GraphWidget* parent)
    : BaseGraphGL(exp, parent),
      m_settingsDlg(new GraphSettings(cMgr, exp, this)),
      m_edgeAttr(-1),
      m_edgeCMap(nullptr),
      m_edgeSizeRate(25.)
{
    setWindowTitle("Graph");

    connect(m_settingsDlg->nodeColorSelector(),
            SIGNAL(cmapUpdated(ColorMap*)), SLOT(setNodeCMap(ColorMap*)));
    connect(m_settingsDlg->edgeColorSelector(),
            SIGNAL(cmapUpdated(ColorMap*)), SLOT(setEdgeCMap(ColorMap*)));
    m_settingsDlg->init();

    m_showNodes = m_ui->bShowNodes->isChecked();
    m_showEdges = m_ui->bShowEdges->isChecked();
    connect(m_ui->bShowNodes, &QPushButton::clicked,
        [this](bool b) { m_showNodes = b; updateCache(); });
    connect(m_ui->bShowEdges, &QPushButton::clicked,
        [this](bool b) { m_showEdges = b; updateCache(); });

    setTrial(0); // init at trial 0
}

CacheStatus GraphView::refreshCache()
{
    if (paintingActive()) {
        return CacheStatus::Scheduled;
    }
    Utils::clearAndShrink(m_cache);
    if (!m_trial || !m_trial->graph() || (!m_showNodes && !m_showEdges)) {
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
        cache.xy = xy;

        if (m_showNodes) {
            cache.node = np.second;
        }

        if (m_showEdges) {
            cache.edges.reserve(np.second.outEdges().size());
            for (auto const& ep : np.second.outEdges()) {
                QPointF xy2(m_origin.x() + edgeSizeRate * (1.0 + ep.second.neighbour().x()),
                            m_origin.y() + edgeSizeRate * (1.0 + ep.second.neighbour().y()));
                QLineF line(xy, xy2);
                if (!m_showNodes || line.length() - m_nodeRadius * 2. > 4.0) {
                    cache.edges.emplace_back(line); // just add visible edges
                }
            }
            cache.edges.shrink_to_fit();
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
            if (!cache.node.isNull() && cache.node.id() == m_selectedNode) {
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

    const double nodeRadius = m_nodeRadius;
    if (m_showNodes && m_nodeAttr >= 0 && m_nodeCMap) {
        for (const Cache& cache : m_cache) {
            if (cache.node.isNull()) {
                break;
            }
            if (cache.node.id() == m_selectedNode) {
                painter.setBrush(QColor(10,10,10,100));
                painter.drawEllipse(cache.xy, nodeRadius*1.5, nodeRadius*1.5);
            }
            const Value& value = cache.node.attr(m_nodeAttr);
            const QColor& color = m_nodeCMap->colorFromValue(value);
            painter.setBrush(color);
            painter.setPen(Qt::black);
            painter.drawEllipse(cache.xy, nodeRadius, nodeRadius);
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

void GraphView::setEdgeCMap(ColorMap* cmap)
{
    m_edgeCMap = cmap;
    m_edgeAttr = cmap ? cmap->attrRange()->id() : -1;
    update();
}

} // evoplex
