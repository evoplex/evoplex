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
#include "ui_graphsettings.h"
#include "utils.h"

namespace evoplex {

GridView::GridView(ColorMapMgr* cMgr, ExperimentPtr exp, GraphWidget* parent)
    : BaseGraphGL(exp, parent),
      m_settingsDlg(new GridSettings(cMgr, exp, this))
{
    setWindowTitle("Grid");

    connect(m_settingsDlg->nodeColorSelector(),
            SIGNAL(cmapUpdated(ColorMap*)), SLOT(setNodeCMap(ColorMap*)));
    m_settingsDlg->init();

    //m_settingsDlg->edges->setHidden(true);
    m_ui->bShowNodes->hide();
    m_ui->bShowEdges->hide();
    setTrial(0); // init at trial 0
}

CacheStatus GridView::refreshCache()
{
    if (paintingActive()) {
        return CacheStatus::Scheduled;
    }
    Utils::clearAndShrink(m_cache);
    if (!m_trial || !m_trial->graph()) {
        return CacheStatus::Ready;
    }

    const Nodes& nodes = m_trial->graph()->nodes();
    m_cache.reserve(nodes.size());

    const double nodeRadius = m_nodeRadius;
    for (auto const& np : nodes) {
        QRectF r(m_origin.x() + np.second.x() * nodeRadius,
                 m_origin.y() + np.second.y() * nodeRadius,
                 nodeRadius, nodeRadius);

        if (!rect().contains(r.x(), r.y()))
            continue;

        Cache c;
        c.node = np.second;
        c.rect = r;
        m_cache.emplace_back(c);
    }
    m_cache.shrink_to_fit();

    return CacheStatus::Ready;
}

void GridView::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), m_background);

    if (m_cacheStatus != CacheStatus::Ready || m_nodeAttr < 0 || !m_nodeCMap) {
        painter.end();
        return;
    }

    for (const Cache& cache : m_cache) {
        QColor color;
        if (m_selectedNode == cache.node.id()) {
            color = QColor(10,10,10,100);
        } else {
            const Value& value = cache.node.attr(m_nodeAttr);
            color = m_nodeCMap->colorFromValue(value);
        }
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawRect(cache.rect);
    }

    painter.end();
}

Node GridView::selectNode(const QPoint& pos) const
{
    if (m_cacheStatus == CacheStatus::Ready) {
        for (const Cache& cache : m_cache) {
            if (cache.rect.contains(pos)) {
                return cache.node;
            }
        }
    }
    return Node();
}

}
