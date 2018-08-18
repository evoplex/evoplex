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

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "basegraphgl.h"
#include "graphsettings.h"

namespace evoplex {

class GraphView : public BaseGraphGL
{
    Q_OBJECT
public:
    explicit GraphView(ColorMapMgr* cMgr, ExperimentPtr exp, GraphWidget* parent);

public slots:
    void openSettings() override { m_settingsDlg->show(); }

protected:
    void paintEvent(QPaintEvent*) override;
    Node selectNode(const QPoint& pos) const override;
    CacheStatus refreshCache() override;

private slots:
    void setEdgeCMap(ColorMap* cmap);

private:
    struct Cache {
        Node node;
        QPointF xy;
        std::vector<QLineF> edges;
    };
    std::vector<Cache> m_cache;
    GraphSettings* m_settingsDlg;

    int m_edgeAttr;
    ColorMap* m_edgeCMap;
    qreal m_edgeSizeRate;

    bool m_showNodes;
    bool m_showEdges;
};

} // evoplex
#endif // GRAPHVIEW_H
