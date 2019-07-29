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

bool StarGraph::init()
{
    return true; // nothing to do
}

bool StarGraph::reset()
{
    removeAllEdges();

    const int nNodes = numNodes();
    double radius = (nNodes - 1) / (2. * M_PI);
    double dTheta = 1. / radius;
    if (radius < 2.0) {
        radius = 2.0;
        dTheta = nNodes < 3 ? 0 : (2. * M_PI) / (nNodes - 1);
    }

    // at this point, it is safe to iterate by node ids,
    // which will always start from 0 and end at size-1
    node(0).setCoords(radius, radius);
    if (m_edgeAttrsGen) {
        auto setOfAttrs = m_edgeAttrsGen->create(nNodes - 1);
        int nodeId = 1;
        for (auto& attrs : setOfAttrs) {
            fixCoords(node(nodeId), radius, dTheta);
            addEdge(0, nodeId, new Attributes(attrs));
            ++nodeId;
        }
    } else {
        for (int nodeId = 1; nodeId < nNodes; ++nodeId) {
            fixCoords(node(nodeId), radius, dTheta);
            addEdge(0, nodeId, new Attributes());
        }
    }

    return true;
}

void StarGraph::fixCoords(Node n, double radius, double dTheta) const
{
    double t = dTheta * (n.id() - 1);
    n.setCoords(radius + radius * qCos(t),
                radius + radius * qSin(t));
}

} // evoplex
REGISTER_PLUGIN(StarGraph)
#include "plugin.moc"
