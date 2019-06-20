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
      m_edgePen(Qt::gray),
      m_nodePen(Qt::black)
{
    m_settingsDlg->init();
    setNodeScale(m_settingsDlg->nodeScale());
    m_edgeScale = m_settingsDlg->edgeScale();

    m_showNodes = m_ui->bShowNodes->isChecked();
    m_showEdges = m_ui->bShowEdges->isChecked();
    connect(m_ui->bShowNodes, &QPushButton::clicked,
        [this](bool b) { m_showNodes = b; updateCache(); });
    connect(m_ui->bShowEdges, &QPushButton::clicked,
        [this](bool b) { m_showEdges = b; updateCache(); });

    updateNodePen();
    m_origin += m_origin; // double margin

    setTrial(0); // init at trial 0
}

GraphView::Star GraphView::createStar(const Node& node,
        const qreal& edgeSizeRate, const QPointF& xy)
{
    Star star;
    star.xy = xy;

    if (m_showNodes) {
        star.node = node;
    }

    if (m_showEdges) {
        star.edges.reserve(node.outEdges().size());
        for (auto const& ep : node.outEdges()) {
            QPointF xy2 = nodePoint(ep.second.neighbour(), edgeSizeRate);
            QLineF line(xy, xy2);
            // just add the visible edges
            if (!m_showNodes || line.length() - m_nodeRadius * 2. > 4.0) {
                star.edges.push_back({ep.second, line});
            }
        }
        star.edges.shrink_to_fit();
    }

    return star;
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

    const qreal edgeSR = currEdgeSize();
    const int m = qRound(edgeSR);
    QRectF frame = rect().translated(-m_origin.toPoint());
    frame = frame.marginsAdded(QMargins(m, m, m, m));

    m_cache.reserve(m_trial->graph()->nodes().size());
    for (auto const& np : m_trial->graph()->nodes()) {
        QPointF xy = nodePoint(np.second, edgeSR);
        if (!frame.contains(xy)) {
            continue;
        }
        m_cache.emplace_back(createStar(np.second, edgeSR, xy));
    }
    m_cache.shrink_to_fit();

    return CacheStatus::Ready;
}

void GraphView::setSelectedNode(const Node& node, bool ctrl)
{
    if (ctrl){
        m_selectedNodeTar = node;
    }
    else {
        m_selectedNodeBase = node;
    }
}

Node GraphView::selectNode(const QPointF& pos, bool center)
{
    m_selectedStar = Star();
    if (m_cacheStatus != CacheStatus::Ready) {
        return Node();
    }

    const QPointF p = pos - m_origin;
    for (const Star& star : m_cache) {
        if (p.x() > star.xy.x()-m_nodeRadius &&
            p.x() < star.xy.x()+m_nodeRadius &&
            p.y() > star.xy.y()-m_nodeRadius &&
            p.y() < star.xy.y()+m_nodeRadius)
        {
            m_selectedStar = star;
            if (center) { m_origin = rect().center() - star.xy; }
            return star.node;
        }
    }
    return Node();
}

bool GraphView::selectNode(const Node& node, bool center)
{
    m_selectedStar = Star();
    if (m_cacheStatus != CacheStatus::Ready) {
        return false;
    }

    const QPointF p = nodePoint(node, currEdgeSize());
    for (const Star& star : m_cache) {
        if (star.xy == p) {
            m_selectedStar = star;
            if (center) { m_origin = rect().center() - p; }
            return true;
        }
    }

    m_selectedStar = createStar(node, currEdgeSize(), p);
    m_origin = rect().center() - m_selectedStar.xy;
    updateCache();
    return true;
}

void GraphView::setEdgeCMap(ColorMap* cmap)
{
    m_edgeCMap = cmap;
    m_edgeAttr = cmap ? cmap->attrRange()->id() : -1;
    update();
}

void GraphView::setEdgeScale(int v)
{
    m_edgeScale = v;
    updateCache();
}

void GraphView::setEdgeWidth(int v)
{
    m_edgePen = QPen(Qt::gray, v);
    update();
}

void GraphView::updateNodePen()
{
    if (m_nodeRadius < 8) {
        m_nodePen = QColor(100,100,100,0);
    } else if (m_nodeRadius < 13) {
        // 255/(13-8)*(x-8)
        m_nodePen = QColor(100,100,100,51*(m_nodeRadius-8.));
    } else {
        m_nodePen = QColor(100,100,100,255);
    }
}

void GraphView::paintFrame(QPainter& painter) const
{
    if (m_selectedStar.node.isNull() && m_selectedNodeTar.isNull()) {
        painter.setOpacity(1.0);
        drawEdges(painter);
    } else {
        painter.setOpacity(0.2);
    }
    const double nodeRadius = m_nodeRadius;
    drawNodes(painter, nodeRadius);
    drawSelectedStar(painter, nodeRadius);
    drawSelectedEdge(painter, nodeRadius);
}

void GraphView::drawNode(QPainter& painter, const Star& s, double r) const
{
    const Value& value = s.node.attr(m_nodeAttr);
    painter.setBrush(m_nodeCMap->colorFromValue(value));
    painter.drawEllipse(s.xy, r, r);
}

void GraphView::drawNodes(QPainter& painter, double nodeRadius) const
{
    if (!m_showNodes || m_nodeAttr < 0 || !m_nodeCMap) {
        return;
    }
    painter.save();
    painter.setPen(m_nodePen);
    for (const Star& star : m_cache) {
        if (star.node.isNull()) {
            break;
        }
        drawNode(painter, star, nodeRadius);
    }
    painter.restore();
}

void GraphView::drawEdges(QPainter& painter) const
{
    if (!m_showEdges) {
        return;
    }
    painter.save();
    if (m_edgeAttr >= 0 && m_edgeCMap) {
        QPen pen = m_edgePen;
        for (const Star& star : m_cache) {
            for (auto const& ep : star.edges) {
                const Value& value = ep.first.attr(m_edgeAttr);
                pen.setColor(m_edgeCMap->colorFromValue(value));
                painter.setPen(m_edgePen);
                painter.drawLine(ep.second);
            }
        }
    } else {
        painter.setPen(m_edgePen);
        for (const Star& star : m_cache) {
            for (auto const& ep : star.edges) {
                painter.drawLine(ep.second);
            }
        }
    }
    painter.restore();
}

void GraphView::drawSelectedEdge(QPainter& painter, double nodeRadius) const
{
    if (m_selectedNodeTar.isNull() || m_selectedNodeBase.isNull()){
        return;
    }
    
    painter.setOpacity(1.0);
    
    const QPointF p1 = nodePoint(m_selectedNodeBase, currEdgeSize());
    const QPointF p2 = nodePoint(m_selectedNodeTar, currEdgeSize());
    
    painter.save();
    // highlight immediate edges
    painter.setPen(QPen(Qt::darkGray, m_edgePen.width() + 3));
    painter.drawLine(p1.x(), p1.y(), p2.x(), p2.y());
    
    // draw selected node
    painter.setPen(m_nodePen);
    
    const Value& value1 = m_selectedNodeBase.attr(m_nodeAttr);
    painter.setBrush(m_nodeCMap->colorFromValue(value1));
    painter.drawEllipse(p1, nodeRadius, nodeRadius);
    
    const Value& value2 = m_selectedNodeTar.attr(m_nodeAttr);
    painter.setBrush(m_nodeCMap->colorFromValue(value2));
    painter.drawEllipse(p2, nodeRadius, nodeRadius);
    
    painter.restore();
}

void GraphView::drawSelectedStar(QPainter& painter, double nodeRadius) const
{
    if (m_selectedStar.node.isNull()) {
        return;
    }

    painter.setOpacity(1.0);

    // draw shadow of the seleted node
    painter.save();
    double shadowRadius = nodeRadius*1.5;
    QRadialGradient r(m_selectedStar.xy, shadowRadius, m_selectedStar.xy);
    r.setColorAt(0, Qt::black);
    r.setColorAt(1, m_background.color());
    painter.setBrush(r);
    painter.setPen(Qt::transparent);
    painter.drawEllipse(m_selectedStar.xy, shadowRadius, shadowRadius);
    painter.restore();

    painter.save();
    // highlight immediate edges
    painter.setPen(QPen(Qt::darkGray, m_edgePen.width() + 3));
    for (auto const& ep : m_selectedStar.edges) {
        painter.drawLine(ep.second);
    }

    // draw selected node
    painter.setPen(m_nodePen);
    drawNode(painter, m_selectedStar, nodeRadius);

    // draw neighbours
    const Edges& oe = m_selectedStar.node.outEdges();
    const double esize = currEdgeSize();
    for (auto const& e : oe) {
        const Node& n = e.second.neighbour();
        Star s(n, nodePoint(n, esize), {});
        drawNode(painter, s, nodeRadius);
    }
    painter.restore();
}

} // evoplex
