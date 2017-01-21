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

// one agent can be linked to N neighbours which are also agents in the graph
typedef AbstractAgent Neighbour; // alias
typedef QVector<Neighbour*> Neighbours;  // neighbourhood of one agent
// A hash to allow us to find out the links of each agent.
// We call it 'network' just to avoid any confusion with the whole 'graph' object.
typedef QHash<int, Neighbours> Network;

class AbstractGraph
{
public:
    // constructor
    explicit AbstractGraph(const QString& name): m_graphName(name) {}

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // Tt is usually used to validate the graphParams and the set of agents.
    virtual bool init(QVector<AbstractAgent*> agents, const QVariantHash& graphParams) = 0;

    // Reset the neighbourhood of all agents to the original structure.
    virtual void resetNetwork() = 0;

    // This method is used to introduce spatial coordinates for each agent.
    // It is mainly used by the GUI when it wants to draw the graph.
    // If returns false, GUI will not draw it.
    virtual bool buildCoordinates() = 0;

    // return the current value of all graph parameters (if any)
    // eg., height, width ...
    virtual QVariantHash getGraphParams() const = 0;

    // getters
    inline const QString& getGraphName() { return m_graphName; }
    inline QVector<AbstractAgent*> getAgents() { return m_agents; }
    inline AbstractAgent* getAgent(int id) { return m_agents.value(id, NULL); }
    inline const Network& getNetwork() { return m_network; }
    inline const Neighbours getNeighbours(int id) { return m_network.value(id); }

protected:
    const QString m_graphName;
    QVector<AbstractAgent*> m_agents;
    Network m_network;
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
    class PG_CLASSNAME: public QObject, public IPluginGraph                 \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginGraph"                        \
                      FILE "graphMetaData.json")                            \
    Q_INTERFACES(IPluginGraph)                                              \
    public:                                                                 \
        QSharedPointer<AbstractGraph> create() {                            \
            return QSharedPointer<AbstractGraph>(new CLASSNAME(CLASSNAME)); \
        }                                                                   \
    };

#endif // ABSTRACT_GRAPH_H
