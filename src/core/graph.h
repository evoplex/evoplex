/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <QHash>
#include <QObject>
#include <QVector>

class IAgent;

class Graph: public QObject
{
    Q_OBJECT

public:
    typedef const IAgent* Neighbour;
    typedef QVector<Neighbour> Neighbours;

    enum GraphType {
        SQUARE_GRID,
        MOORE_GRID
    };

    Graph(QVector<IAgent*> agents, GraphType type);
    Graph(Graph *g);
    virtual ~Graph();

    inline const bool getIsValid() { return m_valid; }
    inline const GraphType getGraphType() { return m_type; }
    inline const int getWidth() { return m_width; }
    inline const int getHeight() { return m_height; }
    inline const QVector<IAgent*> getAgents() { return m_agents; }
    inline const QVector<Neighbours> getNeighbours() { return m_neighbours; }

    inline const IAgent* getAgent(int id) { return m_agents.at(id); }
    inline const Neighbours getNeighbours(int id) { return m_neighbours.at(id); }

private:
    bool m_valid;
    GraphType m_type;
    int m_width;
    int m_height;
    QVector<IAgent*> m_agents;
    QVector<Neighbours> m_neighbours; // each agentId maps to a set of neighbours

    void buildSquareGrid();
    void buildMooreGrid();

    Neighbours calcMooreNeighborhood(const int id) const;
    Neighbours calcSquareNeighborhood(const int id) const;
};

#endif // GRAPH_H
