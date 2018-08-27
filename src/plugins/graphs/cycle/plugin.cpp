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

#include <QtMath>
#include "plugin.h"

namespace evoplex {

bool CycleGraph::init()
{
    return true; // nothing to do
}

bool CycleGraph::reset()
{
    removeAllEdges();

    const double radius = numNodes() / (2. * M_PI);
    const double dTheta = 1. / radius;
    const int lastId = numNodes() - 1;

    // at this point, it is safe to iterate by node ids,
    // which will always start from 0 and end at size-1
    if (m_edgeAttrsGen) {
        auto soa = m_edgeAttrsGen->create(numNodes());
        for (int nodeId = 0; nodeId < lastId; ++nodeId) {
            fixCoords(node(nodeId), radius, dTheta);
            addEdge(nodeId, nodeId+1, new Attributes(soa.at(nodeId)));
        }
        fixCoords(node(lastId), radius, dTheta);
        addEdge(lastId, 0, new Attributes(soa.at(lastId)));
    } else {
        for (int nodeId = 0; nodeId < lastId; ++nodeId) {
            fixCoords(node(nodeId), radius, dTheta);
            addEdge(nodeId, nodeId+1, new Attributes());
        }
        fixCoords(node(lastId), radius, dTheta);
        addEdge(lastId, 0, new Attributes());
    }

    return true;
}

void CycleGraph::fixCoords(Node n, double radius, double dTheta) const
{
    n.setCoords(radius + radius * qCos(dTheta * n.id()),
                radius + radius * qSin(dTheta * n.id()));
}

} // evoplex
REGISTER_PLUGIN(CycleGraph)
#include "plugin.moc"
