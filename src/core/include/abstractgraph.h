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

#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <QtDebug>
#include <QMutex>

#include "abstractplugin.h"
#include "attrsgenerator.h"
#include "edges.h"
#include "enum.h"
#include "nodes.h"
#include "prg.h"

namespace evoplex {

class GraphPlugin;

/**
 * @brief Provides a common interface for Graph plugins.
 * @see AbstractGraph
 */
class AbstractGraphInterface : public AbstractPlugin
{
public:
    //! Provide a default destructor to keep compilers happy.
    virtual ~AbstractGraphInterface() = default;

    using AbstractPlugin::init;

    /**
     * @brief Resets the graph object to the original state.
     * This method is triggered after a successful AbstractPlugin::init().
     * @return true if successful.
     */
    virtual bool reset() = 0;
};

/**
 * @brief Abstract base class for graph plugins.
 */
class AbstractGraph : public AbstractGraphInterface
{
    friend class Trial;

public:
//! @addtogroup GraphAPI
//! @{

    //! @copydoc AbstractPlugin::nodes
    virtual inline PRG* prg() const override;

    /**
     * @brief Gets the graph id.
     */
    const QString& id() const;

    /**
     * @brief Gets the graph type.
     */
    GraphType type() const;

    /**
     * @brief Returns true if the graph is directed.
     */
    inline bool isDirected() const;

    /**
     * @brief Returns true if the graph is undirected.
     */
    inline bool isUndirected() const;

    /**
     * @brief Gets the edges.
     */
    inline const Edges& edges() const;

    /**
     * @brief Returns the Edge corresponding to \p edgeId.
     * @param edgeId A valid edge id.
     * @throw std::out_of_range if no such data is present.
     */
    inline const Edge& edge(int edgeId) const;

    /**
     * @brief Returns the Edge that connects \p originId to \p neighbourId.
     * @param originId A valid node id.
     * @param neighbourId A valid node id.
     * @throw std::out_of_range if no such data is present.
     */
    inline const Edge& edge(int originId, int neighbourId) const;

    /**
     * @brief Gets the nodes.
     */
    inline const Nodes& nodes() const;

    /**
     * @brief Gets the Node corresponding to \p nodeId.
     * @param nodeId A valid node id.
     * @throw std::out_of_range if no such data is present.
     */
    inline Node node(int nodeId) const;

    /**
     * @brief Gets a random Node in the graph.
     * @return If the graph has no nodes, it returns an invalid/empty Node.
     */
    Node randNode() const;

    /**
     * @brief Gets the number of nodes in the graph.
     */
    inline int numNodes() const;

    /**
     * @brief Gets the number of edges in the graph.
     */
    inline int numEdges() const;

    /**
     * @brief Creates a Node with \p attrs and adds it into the graph.
     * @returns the new Node
     * @param attr nodes' attributes
     */
    inline Node addNode(Attributes attr);

    /**
     * @copydoc addNode(Attributes)
     * @param x,y nodes's coordinates
     */
    Node addNode(Attributes attr, float x, float y);

    /**
     * @brief Creates and adds an Edge into the graph.
     * @param originId the id of the source Node
     * @param neighbourId the id of the target Node
     * @param attrs the edge's attributes
     * @returns the new Edge
     * @warning the nodes' ids must belong to the graph.
     */
    inline Edge addEdge(int originId, int neighbourId, Attributes* attrs=new Attributes());

    /**
     * @brief Creates and adds an Edge into the graph.
     * @param origin the source Node
     * @param neighbour the target Node
     * @param attrs the edge's attributes
     * @returns the new Edge
     * @warning the nodes' ids must belong to the graph.
     */
    Edge addEdge(const Node& origin, const Node& neighbour, Attributes* attrs=new Attributes());

    /**
     * @brief Removes all edges of the graph.
     */
    void removeAllEdges();

    /**
     * @brief Removes all edges of the \p node.
     * @param node A Node that belongs to the graph.
     */
    void removeAllEdges(const Node& node);

    /**
     * @brief Removes the \p node from the graph.
     * @param node A Node that belongs to the graph.
     */
    void removeNode(const Node& node);

    /**
     * @brief Removes a node from the graph.
     * @param it An iterator pointing to a Node in the graph to be erased.
     * @return An iterator pointing to the Node immediately following
     *         \p it prior to the Node being erased. If no such
     *         Node exists, end() is returned.
     */
    Nodes::iterator removeNode(Nodes::iterator it);

    /**
     * @brief Removes the \p edge from the graph.
     * @param edge An Edge that belongs to the graph.
     */
    void removeEdge(const Edge& edge);

    /**
     * @brief Removes an edge from the graph.
     * @param it An iterator pointing to an Edge in the graph to be erased.
     * @return An iterator pointing to the Edge immediately following
     *         \p it prior to the Edge being erased. If no such
     *         Edge exists, end() is returned.
     */
    Edges::iterator removeEdge(Edges::iterator it);

/**@}*/

protected:
    AttrsGeneratorPtr m_edgeAttrsGen;
    Edges m_edges;
    Nodes m_nodes;

    //! constructor
    AbstractGraph();

private:
    QString m_graphId;
    GraphType m_graphType;
    PRG* m_prg;

    int m_lastNodeId;
    int m_lastEdgeId;
    QMutex m_mutex;

    std::uniform_int_distribution<int> m_numNodesDist;

    bool setup(const QString& id, GraphType type, PRG& prg,
               AttrsGeneratorPtr edgeGen, Nodes& nodes, const Attributes& attrs);
};


/************************************************************************
   AbstractGraph: Inline member functions
 ************************************************************************/

inline PRG *AbstractGraph::prg() const
{ return m_prg; }

inline const QString& AbstractGraph::id() const
{ return m_graphId; }

inline GraphType AbstractGraph::type() const
{ return m_graphType; }

inline bool AbstractGraph::isDirected() const
{ return m_graphType == GraphType::Directed; }

inline bool AbstractGraph::isUndirected() const
{ return m_graphType == GraphType::Undirected; }

inline const Edges& AbstractGraph::edges() const
{ return m_edges; }

inline const Nodes& AbstractGraph::nodes() const
{ return m_nodes; }

inline const Edge& AbstractGraph::edge(int edgeId) const
{ return m_edges.at(edgeId); }

inline const Edge& AbstractGraph::edge(int originId, int neighbourId) const
{ return m_nodes.at(originId).outEdges().at(neighbourId); }

inline Node AbstractGraph::node(int nodeId) const
{ return m_nodes.at(nodeId); }

inline int AbstractGraph::numEdges() const
{ return static_cast<int>(m_edges.size()); }

inline int AbstractGraph::numNodes() const
{ return static_cast<int>(m_nodes.size()); }

inline Node AbstractGraph::addNode(Attributes attr)
{ return addNode(attr, 0, m_lastNodeId+1); }

inline Edge AbstractGraph::addEdge(int originId, int neighbourId, Attributes* attrs)
{  return addEdge(m_nodes.at(originId), m_nodes.at(neighbourId), attrs); }

} // evoplex
#endif // ABSTRACT_GRAPH_H
