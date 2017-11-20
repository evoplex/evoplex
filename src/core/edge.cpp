/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
