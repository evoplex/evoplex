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

#include "abstractgraph.h"
#include "constants.h"
#include "trial.h"
#include "utils.h"

namespace evoplex {

AbstractGraph::GraphType AbstractGraph::enumFromString(const QString& str)
{
    if (str == "undirected") return Undirected;
    if (str == "directed") return Directed;
    return Invalid_Type;
}

AbstractGraph::AbstractGraph()
    : AbstractPlugin(),
      m_lastNodeId(-1),
      m_lastEdgeId(-1)
{
}

bool AbstractGraph::setup(Trial& trial, const Attributes& attrs, Nodes& nodes)
{
    Q_ASSERT_X(nodes.size() < EVOPLEX_MAX_NODES, "setup", "too many nodes!");
    Q_ASSERT_X(!nodes.empty(), "setup", "set of nodes cannot be empty!");
    m_nodes = nodes;
    m_lastNodeId = static_cast<int>(m_nodes.size());
    return AbstractPlugin::setup(trial, attrs);
}

inline AbstractGraph::GraphType AbstractGraph::type() const
{
    return m_trial->graphType();
}

NodePtr AbstractGraph::addNode(Attributes attr, int x, int y)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastNodeId;
    NodePtr node;
    Node::constructor_key k;
    if (isDirected()) {
        node = std::make_shared<DNode>(k, m_lastNodeId, attr, x, y);
    } else {
        node = std::make_shared<UNode>(k, m_lastNodeId, attr, x, y);
    }
    m_nodes.insert({m_lastNodeId, node});
    return node;
}

EdgePtr AbstractGraph::addEdge(const NodePtr& origin, const NodePtr& neighbour, Attributes* attrs)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastEdgeId;
    Edge::constructor_key k;
    EdgePtr edgeOut = std::make_shared<Edge>(k, m_lastEdgeId, origin, neighbour, attrs, true);
    EdgePtr edgeIn = std::make_shared<Edge>(k, m_lastEdgeId, neighbour, origin, attrs, false);
    origin->addOutEdge(edgeOut);
    neighbour->addInEdge(edgeIn); // neighbour must be aware of the in-connection
    m_edges.insert({m_lastEdgeId, edgeOut}); // store only the original direction
    return edgeOut;
}

void AbstractGraph::removeAllEdges()
{
    QMutexLocker locker(&m_mutex);
    for (auto const& p : m_nodes) {
        p.second->clearInEdges();
        p.second->clearOutEdges();
    }
    m_edges.clear();
}

void AbstractGraph::removeAllEdges(const NodePtr& node)
{
    QMutexLocker locker(&m_mutex);
    if (isUndirected()) {
        for (auto const& p : node->outEdges()) {
            p.second->neighbour()->removeInEdge(p.first);
            m_edges.erase(p.first);
        }
        node->clearOutEdges();
    } else if (isDirected()) {
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

void AbstractGraph::removeNode(const NodePtr& node)
{
    removeAllEdges(node);
    QMutexLocker locker(&m_mutex);
    m_nodes.erase(node->id());
}

Nodes::iterator AbstractGraph::removeNode(Nodes::iterator it)
{
    removeAllEdges((*it).second);
    QMutexLocker locker(&m_mutex);
    return m_nodes.erase(it);
}

void AbstractGraph::removeEdge(const EdgePtr& edge)
{
    QMutexLocker locker(&m_mutex);
    edge->origin()->removeOutEdge(edge->id());
    edge->neighbour()->removeInEdge(edge->id());
    m_edges.erase(edge->id());
}

Edges::iterator AbstractGraph::removeEdge(Edges::iterator it)
{
    QMutexLocker locker(&m_mutex);
    const EdgePtr& edge = (*it).second;
    edge->origin()->removeOutEdge(edge->id());
    edge->neighbour()->removeInEdge(edge->id());
    return m_edges.erase(it);
}

} // evoplex
