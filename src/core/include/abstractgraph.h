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

#include "baseplugin.h"
#include "constants.h"
#include "edge.h"
#include "node.h"
#include "utils.h"

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

    static GraphType enumFromString(const QString& str) {
        if (str == "undirected") return Undirected;
        if (str == "directed") return Directed;
        return Invalid_Type;
    }

    inline const QString& name() const;
    inline const GraphType& type() const;
    inline const QString& typeString() const;
    inline bool isDirected() const;
    inline bool isUndirected() const;

    inline const Edges& edges() const;
    inline const Nodes& nodes() const;
    inline Node* node(int id) const;
    inline Node* randNode() const;

    inline int numNodes() const;
    inline int numEdges() const;

protected:
    Nodes m_nodes;
    Edges m_edges;

    explicit BaseGraph(const QString& name) : BasePlugin(), m_name(name), m_type(Invalid_Type) {}
    virtual ~BaseGraph() {
        Utils::deleteAndShrink(m_nodes);
        Utils::deleteAndShrink(m_edges);
    }

private:
    const QString m_name;
    GraphType m_type;
    QString m_typeStr;

    // takes the ownership of the nodes and the PRG
    inline bool setup(PRG* prg, const Attributes* attrs, Nodes& nodes, const QString& graphType);
};


class AbstractGraph : public BaseGraph
{
public:
    // constructor
    explicit AbstractGraph(const QString& name)
        : BaseGraph(name) {}

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // It is usually used to validate the graph attributes and the set of nodes.
    virtual bool init() = 0;

    // Resets the graph object to the original state.
    // This method is triggered after a successful init()
    virtual void reset() = 0;
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

inline Node* BaseGraph::node(int id) const
{ return m_nodes.at(id); }

inline Node* BaseGraph::randNode() const
{ return m_nodes.at(prg()->randI(m_nodes.size()-1)); }

inline int BaseGraph::numEdges() const
{ return static_cast<int>(m_edges.size()); }

inline int BaseGraph::numNodes() const
{ return static_cast<int>(m_nodes.size()); }

inline bool BaseGraph::setup(PRG* prg, const Attributes* attrs, Nodes& nodes, const QString& graphType) {
    Q_ASSERT_X(nodes.size() < EVOPLEX_MAX_NODES, "BasePlugin::setup", "too many nodes! we cannot handle this.");
    if (BasePlugin::setup(prg, attrs)) {
        m_nodes = nodes;
        m_type = enumFromString(graphType);
        m_typeStr = graphType;
    }
    return !m_nodes.empty() && m_type != Invalid_Type;
}


} // evoplex
#endif // ABSTRACT_GRAPH_H
