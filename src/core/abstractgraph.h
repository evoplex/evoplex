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

class AbstractGraph: public QObject
{
public:
    // constructor
    explicit AbstractGraph(const QString& name)
        : m_graphName(name) { setObjectName(name); }

    // Provide the destructor to keep compilers happy.
    virtual ~AbstractGraph() {}

    // Initializes the graph object.
    // This method is called once when a new graph object is being created.
    // Tt is usually used to validate the graphParams and the set of agents.
    virtual bool init(QVector<AbstractAgent*> agents, QVariantHash graphParams) = 0;

    // Reset the neighbourhood for the current set of agents.
    virtual void resetLinks() = 0;

    // This method is used to introduce spatial coordinates for each agent.
    // It is mainly used by the GUI when it wants to draw the graph.
    // If returns false, GUI will not draw it.
    virtual bool buildCoordinates() = 0;

    // return the current value of all graph parameters (if any)
    // eg., height, width ...
    virtual QVariantHash getGraphParams() const = 0;

    // getters
    inline const QString& getGraphName() { return m_graphName; }
    inline const QVector<AbstractAgent*>& getAgents() { return m_agents; }
    inline AbstractAgent* getAgent(int id) { return m_agents.value(id, NULL); }
    inline const QHash<int, QVector<AbstractAgent*>>& getLinks() { return m_links; }
    inline const QVector<AbstractAgent*> getLinks(int id) { return m_links.value(id); }

protected:
    const QString m_graphName;
    QVector<AbstractAgent*> m_agents;
    QHash<int, QVector<AbstractAgent*>> m_links;
};

class IPluginGraph
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginGraph() {}

    // create the real model object.
    virtual AbstractGraph* create() = 0;

    // author's name
    virtual QString author() = 0;

    // graph's name
    virtual QString name() = 0;

    // graph's description
    virtual QString description() = 0;
};

Q_DECLARE_INTERFACE(AbstractGraph, "org.evoplex.AbstractGraph")
Q_DECLARE_INTERFACE(IPluginGraph, "org.evoplex.IPluginGraph")

#endif // ABSTRACT_GRAPH_H
