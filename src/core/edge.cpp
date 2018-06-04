/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

#include <memory>

#include "agent.h"
#include "attributes.h"
#include "edge.h"

namespace evoplex {

Edge::Edge()
    : m_origin(nullptr)
    , m_neighbour(nullptr)
    , m_attrs(nullptr)
    , m_undirectedEdge(nullptr)
{
}

Edge::Edge(Agent* origin, Agent* neighbour, bool isDirected)
    : Edge(origin, neighbour, new Attributes(), isDirected)
{
}

Edge::Edge(Agent* origin, Agent* neighbour, Attributes* attrs, bool isDirected)
    : m_origin(origin)
    , m_neighbour(neighbour)
    , m_attrs(attrs)
    , m_undirectedEdge(nullptr)
{
    origin->m_edges.emplace_back(this);
    if (!isDirected) {
        m_undirectedEdge = new Edge();
        m_undirectedEdge->m_origin = neighbour;
        m_undirectedEdge->m_neighbour = origin;
        m_undirectedEdge->m_attrs = attrs;
        neighbour->m_edges.emplace_back(m_undirectedEdge);
    }
}

Edge::~Edge()
{
    if (m_undirectedEdge) {
        delete m_undirectedEdge;
    } else {
        delete m_attrs;
    }
}

}
