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

#include "gridview.h"
#include "ui_graphwidget.h"
#include "ui_graphsettings.h"

namespace evoplex
{

GridView::GridView(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent)
    : GraphWidget(mainGUI, exp, parent)
{
    setWindowTitle("Grid");
    //m_settingsDlg->edges->setHidden(true);
    m_ui->bShowAgents->hide();
    m_ui->bShowEdges->hide();
    setTrial(0); // init at trial 0
}

int GridView::refreshCache()
{
    if (paintingActive()) {
        return Scheduled;
    }
    Utils::deleteAndShrink(m_cache);
    if (!m_model) {
        return Ready;
    }

    const Agents& agents = m_model->graph()->agents();
    m_cache.reserve(agents.size());

    for (Agent* agent : agents) {
        QRectF r(m_origin.x() + agent->x() * m_nodeRadius,
                 m_origin.y() + agent->y() * m_nodeRadius,
                 m_nodeRadius, m_nodeRadius);

        if (!rect().contains(r.x(), r.y()))
            continue;

        Cache c;
        c.agent = agent;
        c.rect = r;
        m_cache.emplace_back(c);
    }
    m_cache.shrink_to_fit();

    return Ready;
}

void GridView::paintEvent(QPaintEvent*)
{
    if (m_cacheStatus != Ready) {
        return;
    }

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const Cache& cache : m_cache) {
        QColor color;
        if (m_selectedAgent == cache.agent->id()) {
            color = QColor(10,10,10,100);
        } else {
            const Value& value = cache.agent->attr(m_agentAttr);
            color = m_agentCMap->colorFromValue(value);
        }
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawRect(cache.rect);
    }

    painter.end();
}

const Agent* GridView::selectAgent(const QPoint& pos) const
{
    if (m_cacheStatus == Ready) {
        for (const Cache& cache : m_cache) {
            if (cache.rect.contains(pos)) {
                return cache.agent;
            }
        }
    }
    return nullptr;
}

}
