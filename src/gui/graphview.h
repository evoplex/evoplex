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

#include "graphwidget.h"

namespace evoplex
{

class GraphView : public GraphWidget
{
public:
    explicit GraphView(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent);

protected:
    void paintEvent(QPaintEvent*) override;
    virtual const Node* selectNode(const QPoint& pos) const;

private:
    struct Cache {
        const Node* node = nullptr;
        QPointF xy;
        std::vector<QLineF> edges;
    };
    std::vector<Cache> m_cache;

    int m_edgeAttr;
    ColorMap* m_edgeCMap;
    float m_edgeSizeRate;

    bool m_showNodes;
    bool m_showEdges;

    virtual int refreshCache();
};

} // evoplex
#endif // GRAPHVIEW_H
