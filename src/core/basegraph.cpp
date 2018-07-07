/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#include "basegraph.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

BaseGraph::GraphType BaseGraph::enumFromString(const QString& str)
{
    if (str == "undirected") return Undirected;
    if (str == "directed") return Directed;
    return Invalid_Type;
}

BaseGraph::BaseGraph(const QString& name)
    : BasePlugin(),
      m_name(name),
      m_type(Invalid_Type),
      m_lastNodeId(-1),
      m_lastEdgeId(-1)
{
}

bool BaseGraph::setup(PRG* prg, const Attributes* attrs, Nodes& nodes, const QString& graphType)
{
    Q_ASSERT_X(nodes.size() < EVOPLEX_MAX_NODES, "BasePlugin::setup", "too many nodes! we cannot handle this.");
    if (BasePlugin::setup(prg, attrs)) {
        m_nodes = nodes;
        m_type = enumFromString(graphType);
        m_typeStr = graphType;
        m_lastNodeId = nodes.size();
    }
    return !m_nodes.empty() && m_type != Invalid_Type;
}

NodePtr BaseGraph::addNode(Attributes attr, int x, int y)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastNodeId;
    NodePtr node;
    if (type() == Undirected) {
        node = std::make_shared<UNode>(m_lastNodeId, attr, x, y);
    } else {
        node = std::make_shared<DNode>(m_lastNodeId, attr, x, y);
    }
    m_nodes.insert({m_lastNodeId, node});
    return node;
}

EdgePtr BaseGraph::addEdge(const NodePtr& origin, const NodePtr& neighbour, Attributes* attrs)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastEdgeId;
    EdgePtr edgeOut = std::make_shared<Edge>(m_lastEdgeId, origin, neighbour, attrs, true);
    EdgePtr edgeIn = std::make_shared<Edge>(m_lastEdgeId, neighbour, origin, attrs, false);
    origin->addOutEdge(edgeOut);
    neighbour->addInEdge(edgeIn); // neighbour must be aware of the in-connection
    m_edges.insert({m_lastEdgeId, edgeOut}); // store only the original direction
    return edgeOut;
}

void BaseGraph::removeAllEdges()
{
    QMutexLocker locker(&m_mutex);
    for (auto const& p : m_nodes) {
        p.second->clearInEdges();
        p.second->clearOutEdges();
    }
    m_edges.clear();
}

void BaseGraph::removeAllEdges(const NodePtr& node)
{
    QMutexLocker locker(&m_mutex);
    if (type() == Undirected) {
        for (auto const& p : node->outEdges()) {
            p.second->neighbour()->removeInEdge(p.first);
            m_edges.erase(p.first);
        }
        node->clearOutEdges();
    } else if (type() == Directed) {
        for (auto const& p : node->outEdges()) {
            p.second->neighbour()->removeInEdge(p.first);
            m_edges.erase(p.first);
        }
        for (auto const& p : node->inEdges()) {
            p.second->neighbour()->removeOutEdge(p.first);
            m_edges.erase(p.first);
        }
        node->clearInEdges();
        node->clearOutEdges();
    } else {
        qFatal("invalid type!");
    }
}

void BaseGraph::removeNode(const NodePtr& node)
{
    removeAllEdges(node);
    QMutexLocker locker(&m_mutex);
    m_nodes.erase(node->id());
}

Nodes::iterator BaseGraph::removeNode(Nodes::iterator it)
{
    removeAllEdges((*it).second);
    QMutexLocker locker(&m_mutex);
    return m_nodes.erase(it);
}

void BaseGraph::removeEdge(const EdgePtr& edge)
{
    QMutexLocker locker(&m_mutex);
    edge->origin()->removeOutEdge(edge->id());
    edge->neighbour()->removeInEdge(edge->id());
    m_edges.erase(edge->id());
}

Edges::iterator BaseGraph::removeEdge(Edges::iterator it)
{
    QMutexLocker locker(&m_mutex);
    const EdgePtr& edge = (*it).second;
    edge->origin()->removeOutEdge(edge->id());
    edge->neighbour()->removeInEdge(edge->id());
    return m_edges.erase(it);
}

} // evoplex
