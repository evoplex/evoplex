/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <QHash>
#include <QObject>
#include <QVector>

class AbstractAgent;

class Graph: public QObject
{
    Q_OBJECT

public:
    typedef const AbstractAgent* Neighbour;
    typedef QVector<Neighbour> Neighbours;

    enum GraphType {
        SQUARE_GRID,
        MOORE_GRID,
        ERROR
    };
    static GraphType graphTypeFromString(QString type);
    static QString graphTypeToString(GraphType type);

    Graph(QVector<AbstractAgent*> agents, GraphType type);
    Graph(QVector<AbstractAgent*> agents, QString type): Graph(agents, graphTypeFromString(type)) {}
    Graph(Graph *g);
    virtual ~Graph();

    inline const bool isValid() { return m_valid; }
    inline const GraphType getGraphType() { return m_type; }
    inline const int getWidth() { return m_width; }
    inline const int getHeight() { return m_height; }
    inline const QVector<AbstractAgent*> getAgents() { return m_agents; }
    inline const QVector<Neighbours> getNeighbours() { return m_neighbours; }

    inline const AbstractAgent* getAgent(int id) { return m_agents.at(id); }
    inline const Neighbours getNeighbours(int id) { return m_neighbours.at(id); }

private:
    bool m_valid;
    GraphType m_type;
    int m_width;
    int m_height;
    QVector<AbstractAgent*> m_agents;
    QVector<Neighbours> m_neighbours; // each agentId maps to a set of neighbours

    void buildSquareGrid();
    void buildMooreGrid();

    Neighbours calcMooreNeighborhood(const int id) const;
    Neighbours calcSquareNeighborhood(const int id) const;
};

#endif // GRAPH_H
