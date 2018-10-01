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

namespace evoplex {

/**
 *  @defgroup AbstractGraph The Second Group
 *  This is the second group
 */

/**
 * @brief Provides a common interface for Graph plugins.
 * @see AbstractGraph
 * @ingroup AbstractGraph
 */
class AbstractGraphInterface : public AbstractPlugin
{
public:
    //! Provide a default destructor to keep compilers happy.
    virtual ~AbstractGraphInterface() = default;

    //! @see AbstractPlugin::init()
    using AbstractPlugin::init;

    /**
     * @brief Resets the graph object to the original state.
     * This method is triggered after a successful AbstractPlugin::init().
     * @return true if successful.
     */
    virtual bool reset() = 0;
};

/**
 * @brief Abstract base class for Graph plugins.
 * @ingroup AbstractGraph
 */
class AbstractGraph : public AbstractGraphInterface
{
    friend class Trial;

public:
    //! @returns the graph id
    const QString& id() const;

    //! @returns the graph type (e.g., directed or undirected)
    GraphType type() const;
    //! @returns true if the graph is directed
    inline bool isDirected() const;
    //! @returns true if the graph is undirected
    inline bool isUndirected() const;

    //! @returns the reference of the container
    //! containing all the edges of the graph
    inline const Edges& edges() const;
    //! @returns the Edge corresponding to \p edgeId
    //! @throw  std::out_of_range if no such data is present.
    inline const Edge& edge(int edgeId) const;
    //! @returns the Edge that connects \p originId to \p neighbourId
    //! @throw  std::out_of_range if no such data is present.
    inline const Edge& edge(int originId, int neighbourId) const;

    //! @returns the reference of the container
    //! containing all the nodes of the graph
    inline const Nodes& nodes() const;
    //! @returns the Node corresponding to \p nodeId
    //! @throw  std::out_of_range if no such data is present.
    inline Node node(int nodeId) const;
    //! @returns a random Node in the graph
    Node randNode() const;

    //! @returns the number of nodes in the graph
    inline int numNodes() const;
    //! @returns the number of edges in the graph
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

    //! @brief Removes all edges of the graph.
    void removeAllEdges();
    //! @brief Removes all edges of the \p node.
    void removeAllEdges(const Node& node);

    //! @brief Removes the \p node from the graph.
    void removeNode(const Node& node);
    //! @brief Removes a node from the graph.
    Nodes::iterator removeNode(Nodes::iterator it);

    //! @brief Removes the \p edge from the graph.
    void removeEdge(const Edge& edge);
    //! @brief Removes an edge from the graph.
    Edges::iterator removeEdge(Edges::iterator it);

protected:
    AttrsGeneratorPtr m_edgeAttrsGen;
    Edges m_edges;
    Nodes m_nodes;

    AbstractGraph();

private:
    int m_lastNodeId;
    int m_lastEdgeId;
    QMutex m_mutex;

    std::uniform_int_distribution<int> m_numNodesDist;

    bool setup(Trial& trial, AttrsGeneratorPtr edgeGen,
               const Attributes& attrs, Nodes& nodes);
};


/************************************************************************
   AbstractGraph: Inline member functions
 ************************************************************************/

inline bool AbstractGraph::isDirected() const
{ return type() == GraphType::Directed; }

inline bool AbstractGraph::isUndirected() const
{ return type() == GraphType::Undirected; }

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
