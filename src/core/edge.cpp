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
    , m_isDirected(true)
{
}

Edge::Edge(Agent* origin, Agent* neighbour, Attributes* attrs, bool isDirected)
    : m_origin(origin)
    , m_neighbour(neighbour)
    , m_attrs(attrs)
    , m_isDirected(isDirected)
{
    origin->m_edges.emplace_back(this);
    if (!isDirected) {
        std::shared_ptr<Edge> edge = std::make_shared<Edge>();
        edge->m_origin = neighbour;
        edge->m_neighbour = origin;
        edge->m_attrs = attrs;
        edge->m_isDirected = false;
        neighbour->m_edges.emplace_back(edge.get());
    }
}

Edge::~Edge()
{
    if (m_isDirected) {
        delete m_attrs;
        m_attrs = nullptr;
    }
}

}
