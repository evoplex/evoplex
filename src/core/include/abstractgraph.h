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

#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <QtDebug>
#include <QMutex>

#include "abstractplugin.h"
#include "edges.h"
#include "enum.h"
#include "nodes.h"

namespace evoplex {

class AbstractGraphInterface
{
public:
    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraphInterface() = default;

    // Resets the graph object to the original state.
    // This method is triggered after a successful init()
    // Return true if successful
    virtual bool reset() = 0;
};

class AbstractGraph : public AbstractPlugin, public AbstractGraphInterface
{
    friend class Trial;

public:
    GraphType type() const;
    inline bool isDirected() const;
    inline bool isUndirected() const;

    inline const Edges& edges() const;
    inline const Edge &edge(int id) const;
    inline const Nodes& nodes() const;
    inline Node node(int id) const;
    inline Node randNode() const;

    inline int numNodes() const;
    inline int numEdges() const;

    inline Node addNode(Attributes attr);
    Node addNode(Attributes attr, int x, int y);

    // nodes must belong to the graph
    inline Edge addEdge(const int originId, const int neighbourId, Attributes* attrs=new Attributes());
    Edge addEdge(const Node& origin, const Node& neighbour, Attributes* attrs=new Attributes());

    void removeAllEdges();
    void removeAllEdges(const Node& node);

    void removeNode(const Node& node);
    Nodes::iterator removeNode(Nodes::iterator it);

    void removeEdge(const Edge& edge);
    Edges::iterator removeEdge(Edges::iterator it);

protected:
    Edges m_edges;
    Nodes m_nodes;

    AbstractGraph();
    ~AbstractGraph() override = default;

    bool setup(Trial& trial, const Attributes& attrs, Nodes& nodes);

private:
    int m_lastNodeId;
    int m_lastEdgeId;
    QMutex m_mutex;
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

inline const Edge& AbstractGraph::edge(int id) const
{ return m_edges.at(id); }

inline Node AbstractGraph::node(int id) const
{ return m_nodes.at(id); }

inline Node AbstractGraph::randNode() const
{ return m_nodes.at(prg()->randI(numNodes()-1)); }

inline int AbstractGraph::numEdges() const
{ return static_cast<int>(m_edges.size()); }

inline int AbstractGraph::numNodes() const
{ return static_cast<int>(m_nodes.size()); }

inline Node AbstractGraph::addNode(Attributes attr)
{ return addNode(attr, 0, m_lastNodeId+1); }

inline Edge AbstractGraph::addEdge(const int originId, const int neighbourId, Attributes* attrs)
{  return addEdge(m_nodes.at(originId), m_nodes.at(neighbourId), attrs); }

} // evoplex
#endif // ABSTRACT_GRAPH_H
