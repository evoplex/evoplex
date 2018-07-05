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

#ifndef BASE_GRAPH_H
#define BASE_GRAPH_H

#include <QtDebug>
#include <QMutex>

#include "baseplugin.h"
#include "edge.h"
#include "nodes.h"

namespace evoplex {

class BaseGraph : public BasePlugin
{
    friend class Experiment;

public:
    enum GraphType {
        Invalid_Type = 0,
        Undirected = 1,
        Directed = 2
    };

    static GraphType enumFromString(const QString& str);

    inline const QString& name() const;
    inline const GraphType& type() const;
    inline const QString& typeString() const;
    inline bool isDirected() const;
    inline bool isUndirected() const;

    inline const Edges& edges() const;
    inline const Nodes& nodes() const;
    inline const NodePtr& node(int id) const;
    inline const NodePtr& randNode() const;

    inline int numNodes() const;
    inline int numEdges() const;

    inline NodePtr addNode(Attributes attr);
    NodePtr addNode(Attributes attr, int x, int y);

    // nodes must belong to the graph
    inline EdgePtr addEdge(const int originId, const int neighbourId, Attributes* attrs = new Attributes());
    EdgePtr addEdge(const NodePtr& origin, const NodePtr& neighbour, Attributes* attrs = new Attributes());

    void removeAllEdges();
    void removeAllEdges(const NodePtr& node);

    void removeNode(const NodePtr& node);
    Nodes::iterator removeNode(Nodes::iterator it);

    void removeEdge(const EdgePtr& edge);
    Edges::iterator removeEdge(Edges::iterator it);

protected:
    Nodes m_nodes;
    Edges m_edges;

    explicit BaseGraph(const QString& name);
    virtual ~BaseGraph() {}

private:
    const QString m_name;
    GraphType m_type;
    QString m_typeStr;
    int m_lastNodeId;
    int m_lastEdgeId;
    QMutex m_mutex;

    // takes the ownership of the PRG
    // cannot be called twice
    bool setup(PRG* prg, const Attributes* attrs, Nodes& nodes, const QString& graphType);
};


/************************************************************************
   BaseGraph: Inline member functions
 ************************************************************************/

inline const QString& BaseGraph::name() const
{ return m_name; }

inline const BaseGraph::GraphType& BaseGraph::type() const
{ return m_type; }

inline const QString& BaseGraph::typeString() const
{ return m_typeStr; }

inline bool BaseGraph::isDirected() const
{ return m_type == Directed; }

inline bool BaseGraph::isUndirected() const
{ return m_type == Undirected; }

inline const Edges& BaseGraph::edges() const
{ return m_edges; }

inline const Nodes& BaseGraph::nodes() const
{ return m_nodes; }

inline const NodePtr& BaseGraph::node(int id) const
{ return m_nodes.at(id); }

inline const NodePtr& BaseGraph::randNode() const
{ return m_nodes.at(prg()->randI(m_nodes.size()-1)); }

inline int BaseGraph::numEdges() const
{ return static_cast<int>(m_edges.size()); }

inline int BaseGraph::numNodes() const
{ return static_cast<int>(m_nodes.size()); }

inline NodePtr BaseGraph::addNode(Attributes attr)
{ return addNode(attr, 0, m_lastNodeId+1); }

inline EdgePtr BaseGraph::addEdge(const int originId, const int neighbourId, Attributes* attrs)
{  return addEdge(m_nodes.at(originId), m_nodes.at(neighbourId), attrs); }

} // evoplex
#endif // BASE_GRAPH_H
