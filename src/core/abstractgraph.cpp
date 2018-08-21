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
#include "edge_p.h"
#include "node_p.h"
#include "trial.h"
#include "utils.h"

namespace evoplex {

AbstractGraph::AbstractGraph()
    : m_lastNodeId(-1),
      m_lastEdgeId(-1)
{
}

bool AbstractGraph::setup(Trial& trial, AttrsGeneratorPtr edgeGen,
                          const Attributes& attrs, Nodes& nodes)
{
    Q_ASSERT_X(nodes.size() < EVOPLEX_MAX_NODES, "setup", "too many nodes!");
    Q_ASSERT_X(!nodes.empty(), "setup", "set of nodes cannot be empty!");
    m_nodes = nodes;
    m_lastNodeId = static_cast<int>(m_nodes.size());
    m_edgeAttrsGen = std::move(edgeGen);
    return AbstractPlugin::setup(trial, attrs);
}

const QString& AbstractGraph::id() const
{
    return m_trial->graphId();
}

GraphType AbstractGraph::type() const
{
    return m_trial->graphType();
}

Node AbstractGraph::addNode(Attributes attr, int x, int y)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastNodeId;
    Node node;
    BaseNode::constructor_key k;
    if (isDirected()) {
        node.m_ptr = std::make_shared<DNode>(k, m_lastNodeId, attr, x, y);
    } else {
        node.m_ptr = std::make_shared<UNode>(k, m_lastNodeId, attr, x, y);
    }
    m_nodes.insert({m_lastNodeId, node});
    return node;
}

Edge AbstractGraph::addEdge(const Node& origin, const Node& neighbour, Attributes* attrs)
{
    QMutexLocker locker(&m_mutex);
    ++m_lastEdgeId;
    Edge edgeOut, edgeIn;
    BaseEdge::constructor_key k;
    edgeOut.m_ptr = std::make_shared<BaseEdge>(k, m_lastEdgeId, origin, neighbour, attrs, true);
    edgeIn.m_ptr = std::make_shared<BaseEdge>(k, m_lastEdgeId, neighbour, origin, attrs, false);
    origin.m_ptr->addOutEdge(edgeOut);
    neighbour.m_ptr->addInEdge(edgeIn); // neighbour must be aware of the in-connection
    m_edges.insert({m_lastEdgeId, edgeOut}); // store only the original direction
    return edgeOut;
}

void AbstractGraph::removeAllEdges()
{
    QMutexLocker locker(&m_mutex);
    for (auto const& p : m_nodes) {
        p.second.m_ptr->clearInEdges();
        p.second.m_ptr->clearOutEdges();
    }
    m_edges.clear();
}

void AbstractGraph::removeAllEdges(const Node& node)
{
    QMutexLocker locker(&m_mutex);
    if (isUndirected()) {
        for (auto const& p : node.outEdges()) {
            p.second.neighbour().m_ptr->removeInEdge(p.first);
            m_edges.erase(p.first);
        }
        node.m_ptr->clearOutEdges();
    } else if (isDirected()) {
        for (auto const& p : node.outEdges()) {
            p.second.neighbour().m_ptr->removeInEdge(p.first);
            m_edges.erase(p.first);
        }
        for (auto const& p : node.inEdges()) {
            p.second.neighbour().m_ptr->removeOutEdge(p.first);
            m_edges.erase(p.first);
        }
        node.m_ptr->clearInEdges();
        node.m_ptr->clearOutEdges();
    } else {
        qFatal("invalid type!");
    }
}

void AbstractGraph::removeNode(const Node& node)
{
    removeAllEdges(node);
    QMutexLocker locker(&m_mutex);
    m_nodes.erase(node.id());
}

Nodes::iterator AbstractGraph::removeNode(Nodes::iterator it)
{
    removeAllEdges(it->second);
    QMutexLocker locker(&m_mutex);
    return m_nodes.erase(it);
}

void AbstractGraph::removeEdge(const Edge& edge)
{
    QMutexLocker locker(&m_mutex);
    edge.origin().m_ptr->removeOutEdge(edge.id());
    edge.neighbour().m_ptr->removeInEdge(edge.id());
    m_edges.erase(edge.id());
}

Edges::iterator AbstractGraph::removeEdge(Edges::iterator it)
{
    QMutexLocker locker(&m_mutex);
    const Edge& edge = it->second;
    edge.origin().m_ptr->removeOutEdge(edge.id());
    edge.neighbour().m_ptr->removeInEdge(edge.id());
    return m_edges.erase(it);
}

} // evoplex
