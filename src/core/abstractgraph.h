/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <QObject>
#include <QVariantHash>
#include <QVector>
#include <QtPlugin>

#include "core/abstractagent.h"
#include "core/edge.h"
#include "utils/prg.h"

typedef QHash<AbstractAgent*, Edges*> AdjacencyList;

class AbstractBaseGraph
{
    friend class Experiment;

public:
    inline const QString& getGraphName() const { return m_graphName; }
    inline const Agents& getAgents() const { return m_agents; }
    inline const AdjacencyList& getAdjacencyList() const { return m_adjacencyList; }

    inline AbstractAgent* getAgent(int id) const { return m_agents.at(id); }
    inline AbstractAgent* getRandomAgent() const {
        return m_agents.at(m_prg->randI(m_agents.size()));
    }

protected:
    explicit AbstractBaseGraph(const QString& name): m_graphName(name), m_prg(nullptr) {}

    virtual ~AbstractBaseGraph() {
        setAdjacencyList(AdjacencyList());
        qDeleteAll(m_agents);
        m_agents.clear();
        m_agents.squeeze();
    }

    inline void setAdjacencyList(AdjacencyList adjacencylist) {
        // delete all edges
        AdjacencyList::iterator it = m_adjacencyList.begin();
        while (it != m_adjacencyList.end()) {
            it.value()->clear();
            it.value()->squeeze();
            delete it.value();
            ++it;
        }
        // clear the QHash but do not touch the agents
        m_adjacencyList.clear();
        m_adjacencyList.squeeze();
        // assign to the new adjacency list
        m_adjacencyList = adjacencylist;
    }

private:
    const QString m_graphName;
    Agents m_agents;
    PRG* m_prg;
    AdjacencyList m_adjacencyList;

    // takes the ownership of the agents
    inline void setup(PRG* prg, Agents agents) {
        Q_ASSERT(!m_prg && m_agents.isEmpty()); // make sure it'll be called only once
        m_prg = prg;
        m_agents = agents;
    }
};


class AbstractGraph : public AbstractBaseGraph
{
public:
    // constructor
    explicit AbstractGraph(const QString& name): AbstractBaseGraph(name) {}

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // It is usually used to validate the graphParams and the set of agents.
    virtual bool init(const Attributes& graphParams) = 0;

    // Resets the graph object to the original state.
    virtual void reset() = 0;

    // return the current value of all graph parameters (if any)
    // eg., height, width ...
    virtual QVariantHash getGraphParams() const = 0;
};


class IPluginGraph
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginGraph() {}

    // create the real graph object.
    virtual AbstractGraph* create() = 0;
};
Q_DECLARE_INTERFACE(IPluginGraph, "org.evoplex.IPluginGraph")


#define REGISTER_GRAPH(CLASSNAME)                                           \
    class PG_##CLASSNAME: public QObject, public IPluginGraph               \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginGraph"                        \
                      FILE "graphMetaData.json")                            \
    Q_INTERFACES(IPluginGraph)                                              \
    public:                                                                 \
        AbstractGraph* create() {                                           \
            return dynamic_cast<AbstractGraph*>(new CLASSNAME(#CLASSNAME)); \
        }                                                                   \
    };

#endif // ABSTRACT_GRAPH_H
