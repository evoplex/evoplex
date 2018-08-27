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

#include <functional>

#include "plugin.h"

namespace evoplex {

bool PathGraph::init()
{
    QString layout = attr("layout", "horizontal").toQString();
    if (layout == "horizontal") m_layout = Horizontal;
    else if (layout == "vertical") m_layout = Vertical;
    else if (layout == "none") m_layout = None;
    else return false;

    return true;
}

bool PathGraph::reset()
{
    removeAllEdges();

    // a path graph has n-1 edges
    const int numEdges = numNodes() - 1;

    std::function<void(Node)> fixCoords;
    if (m_layout == Horizontal) {
        fixCoords = [](Node n) { n.setCoords(n.id(), 0); };
    } else if (m_layout == Vertical) {
        fixCoords = [](Node n) { n.setCoords(0, n.id()); };
    } else {
        fixCoords = [](Node) {};
    }

    // at this point, it is safe to iterate by node ids,
    // which will always start from 0 and end at size-1
    if (m_edgeAttrsGen) {
        auto soa = m_edgeAttrsGen->create(numEdges);
        for (int nodeId = 0; nodeId < numEdges; ++nodeId) {
            fixCoords(node(nodeId));
            addEdge(nodeId, nodeId+1, new Attributes(soa.at(nodeId)));
        }
    } else {
        for (int nodeId = 0; nodeId < numEdges; ++nodeId) {
            fixCoords(node(nodeId));
            addEdge(nodeId, nodeId+1, new Attributes());
        }
    }
    // last node
    fixCoords(node(numNodes() - 1));

    return true;
}

} // evoplex
REGISTER_PLUGIN(PathGraph)
#include "plugin.moc"
