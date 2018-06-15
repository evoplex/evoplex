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

#include "agent.h"
#include "constants.h"
#include "edge.h"
#include "prg.h"
#include "utils.h"

namespace evoplex {

class AbstractBaseGraph
{
    friend class AbstractGraph;
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
    inline const Attributes* attrs() const;
    inline Edges edges() const;
    inline Agents agents() const;
    inline Agent* agent(int id) const;
    inline Agent* randAgent() const;

protected:
    Agents m_agents;
    Edges m_edges;

private:
    const QString m_name;
    GraphType m_type;
    const Attributes* m_attrs;
    PRG* m_prg;

    explicit AbstractBaseGraph(const QString& name)
        : m_name(name), m_type(Invalid_Type), m_attrs(nullptr), m_prg(nullptr) {}

    virtual ~AbstractBaseGraph() {
        Utils::deleteAndShrink(m_agents);
        Utils::deleteAndShrink(m_edges);
    }

    // takes the ownership of the agents
    inline bool setup(PRG* prg, Agents& agents, const Attributes* attrs, const QString& graphType);
};


class AbstractGraph : public AbstractBaseGraph
{
public:
    // constructor
    explicit AbstractGraph(const QString& name)
        : AbstractBaseGraph(name) {}

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // It is usually used to validate the graph attributes and the set of agents.
    virtual bool init() = 0;

    // Resets the graph object to the original state.
    // This method is triggered after a successful init()
    virtual void reset() = 0;
};


/************************************************************************
   AbstractBaseGraph: Inline member functions
 ************************************************************************/

inline const QString& AbstractBaseGraph::name() const
{ return m_name; }

inline const AbstractBaseGraph::GraphType& AbstractBaseGraph::type() const
{ return m_type; }

inline const Attributes* AbstractBaseGraph::attrs() const
{ return m_attrs; }

inline Edges AbstractBaseGraph::edges() const
{ return m_edges; }

inline Agents AbstractBaseGraph::agents() const
{ return m_agents; }

inline Agent* AbstractBaseGraph::agent(int id) const
{ return m_agents.at(id); }

inline Agent* AbstractBaseGraph::randAgent() const
{ return m_agents.at(m_prg->randI(m_agents.size()-1)); }

inline bool AbstractBaseGraph::setup(PRG* prg, Agents& agents, const Attributes* attrs, const QString& graphType) {
    // make sure it'll be called only once
    assert(!m_prg && m_agents.empty());
    m_prg = prg;
    m_agents = agents;
    m_attrs = attrs;
    m_type = enumFromString(graphType);
    if (!m_prg || m_agents.empty() || m_type == Invalid_Type) {
        return false;
    }
    return true;
}


} // evoplex
#endif // ABSTRACT_GRAPH_H
