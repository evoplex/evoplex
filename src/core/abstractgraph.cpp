/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "abstractgraph.h"
#include "constants.h"
#include "edge_p.h"
#include "graphplugin.h"
#include "node_p.h"
#include "trial.h"
#include "utils.h"

namespace evoplex {

AbstractGraph::AbstractGraph()
    : m_graphType(GraphType::Invalid),
      m_prg(nullptr),
      m_lastNodeId(-1),
      m_lastEdgeId(-1)
{
}

bool AbstractGraph::setup(const QString& id, GraphType type, PRG& prg,
                          AttrsGeneratorPtr edgeGen, Nodes& nodes, const Attributes& attrs)
{
    Q_ASSERT_X(type != GraphType::Invalid, "setup", "Graph type cannot be invalid!");
    Q_ASSERT_X(nodes.size() < EVOPLEX_MAX_NODES, "setup", "too many nodes!");
    Q_ASSERT_X(!nodes.empty(), "setup", "set of nodes cannot be empty!");
    m_graphId = id;
    m_graphType = type;
    m_nodes = nodes;
    m_prg = &prg;
    m_numNodesDist = std::uniform_int_distribution<int>(0, numNodes()-1);
    m_lastNodeId = static_cast<int>(m_nodes.size());
    m_edgeAttrsGen = std::move(edgeGen);
    return AbstractPlugin::setup(attrs);
}

Node AbstractGraph::randNode() const
{
    if (m_nodes.empty()) {
        return Node();
    }
    return std::next(m_nodes.cbegin(), m_prg->uniform(m_numNodesDist))->second;
}

Node AbstractGraph::addNode(Attributes attr, float x, float y)
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
    m_numNodesDist = std::uniform_int_distribution<int>(0, numNodes()-1);
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
    int sz = m_nodes.empty() ? 0 : numNodes()-1;
    m_numNodesDist = std::uniform_int_distribution<int>(0, sz);
}

Nodes::iterator AbstractGraph::removeNode(Nodes::iterator it)
{
    removeAllEdges(it->second);
    QMutexLocker locker(&m_mutex);
    it = m_nodes.erase(it);
    int sz = m_nodes.empty() ? 0 : numNodes()-1;
    m_numNodesDist = std::uniform_int_distribution<int>(0, sz);
    return it;
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
