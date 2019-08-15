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

#include "gridview.h"
#include "ui_basegraphgl.h"
#include "utils.h"

namespace evoplex {

GridView::GridView(QWidget* parent)
    : BaseGraphGL(parent)
{
    m_ui->bShowNodes->hide();
    m_ui->bShowEdges->hide();
}

CacheStatus GridView::refreshCache()
{
    if (paintingActive()) {
        return CacheStatus::Scheduled;
    }
    Utils::clearAndShrink(m_cache);
    if (!m_abstractGraph) {
        return CacheStatus::Ready;
    }

    const Nodes& nodes = m_abstractGraph->nodes();
    m_cache.reserve(nodes.size());

    const double nodeRadius = m_nodeRadius;
    const int m = qRound(nodeRadius * 2.0);
    QRectF frame = rect().translated(-m_origin.toPoint());
    frame = frame.marginsAdded(QMargins(m, m, m, m));

    for (auto const& np : nodes) {
        QRectF r = cellRect(np.second, nodeRadius);
        if (!frame.contains(r.x(), r.y())) {
            continue;
        }

        Cell c;
        c.node = np.second;
        c.rect = r;
        m_cache.emplace_back(c);
    }
    m_cache.shrink_to_fit();

    return CacheStatus::Ready;
}

void GridView::paintFrame(QPainter& painter) const
{
    if (m_nodeAttr < 0 || !m_nodeCMap) {
        return;
    }

    painter.setOpacity(m_selectedCell.node.isNull() ? 1.0 : 0.2);
    painter.setPen(Qt::transparent);

    for (const Cell& cell : m_cache) {
        if (cell.node.isNull()) {
            break;
        }
        drawCell(painter, cell);
    }

    for (auto cell : m_selectedCells) {
        painter.setOpacity(1.0);
        // draw neighbours
        for (auto const& n : cell.second.node.outEdges()) {
            drawCell(painter, { n, cellRect(n, m_nodeRadius) });
        }
    }
    for (auto cell : m_selectedCells) {
        // draw selected nodes
        drawCell(painter, cell.second);
        painter.setBrush(QBrush(m_background.color(), Qt::DiagCrossPattern));
        painter.drawRect(cell.second.rect);
    }
}

Node GridView::findNode(const QPointF& pos) const
{
    if (m_cacheStatus != CacheStatus::Ready) {
        return Node();
    }

    const QPointF p = pos - m_origin;
    for (const Cell& cell : m_cache) {
        if (cell.rect.contains(p)) {
            return cell.node;
        }
    }
    return Node();
}

Node GridView::selectNode(const QPointF& pos, bool center)
{
    m_selectedCell = Cell();
    if (m_cacheStatus != CacheStatus::Ready) {
        return Node();
    }

    const QPointF p = pos - m_origin;
    for (const Cell& cell : m_cache) {
        if (cell.rect.contains(p)) {
            m_selectedCell = cell;
            if (center) { m_origin = rect().center() - cell.rect.center(); }
            m_selectedCells.insert(std::make_pair(cell.node.id(), cell));
            m_selectedNodes.insert(std::make_pair(cell.node.id(), cell.node));

            return cell.node;
        }
    }

    return Node();
}

bool GridView::selectNode(const Node& node, bool center)
{
    m_selectedCell = Cell();
    if (m_cacheStatus != CacheStatus::Ready) {
        return false;
    }

    const QRectF p = cellRect(node, m_nodeRadius);
    for (const Cell& cell : m_cache) {
        if (cell.rect == p) {
            m_selectedCell = cell;
            if (center) { m_origin = rect().center() - p.center(); }
            return true;
        }
    }

    m_selectedCell = {node, p};
    m_origin = rect().center() - p.center();
    updateCache();
    return true;
}

bool GridView::deselectNode(const Node& node){
    if (m_cacheStatus != CacheStatus::Ready) {
        return false;
    }

    if (inSelectedNodes(node)) {
        m_selectedNodes.erase(node.id());
        m_selectedCells.erase(node.id());
        return true;
    } else {
        return false;
    }
}

void GridView::drawCell(QPainter& painter, const Cell& cell) const
{
    const Value& value = cell.node.attr(m_nodeAttr);
    painter.setBrush(m_nodeCMap->colorFromValue(value));
    painter.drawRect(cell.rect);
}

} // evoplex
