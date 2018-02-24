/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QtMath>

#include "graph.h"

namespace evoplex {

SquareGrid::SquareGrid(const QString& name)
    : AbstractGraph(name)
    , m_numNeighbours(0)
    , m_height(0)
    , m_width(0)
{
}

bool SquareGrid::init()
{
    m_numNeighbours = attrs()->value(Neighbours).toInt();
    if (m_numNeighbours != 4 && m_numNeighbours != 8) {
        qWarning() << QString("The neighbourhood set is invalid (%1)."
                              "It should be 4 or 8").arg(m_numNeighbours);
        return false;
    }

    m_height = attrs()->value(Height).toInt();
    m_width = attrs()->value(Width).toInt();
    if (agents().size() != m_height * m_width) {
        qWarning() << "[SquareGrid]: the agent set is not compatible with the required shape."
                   << "The number of agents should be equal to 'height'*'width'.";
        return false;
    }

    return true;
}

void SquareGrid::reset()
{
    Utils::deleteAndShrink(m_edges);

    bool isDirected;
    edgesFunc func;
    if (type() == Directed) {
        isDirected = true;
        if (m_numNeighbours == 4) {
            func = directed4Edges;
        } else {
            func = directed8Edges;
        }
    } else {
        isDirected = false;
        if (m_numNeighbours == 4) {
            func = undirected4Edges;
        } else {
            func = undirected8Edges;
        }
    }

    for (int id = 0; id < m_agents.size(); ++id) {
        int x, y;
        Utils::ind2sub(id, m_width, y, x);
        m_agents.at(id)->setCoords(x, y);
        createEdges(id, func, isDirected);
    }
}

void SquareGrid::createEdges(const int id, edgesFunc func, bool isDirected)
{
    edges2d neighbors = func(id, m_width);
    for (std::pair<int,int> neighbor : neighbors) {
        if (neighbor.first < 0) {
            neighbor.first = m_height - 1;
        } else if (neighbor.first > m_height - 1) {
            neighbor.first = 0;
        }

        if (neighbor.second < 0) {
            neighbor.second = m_width - 1;
        } else if (neighbor.second > m_width - 1) {
            neighbor.second = 0;
        }

        int nId = Utils::linearIdx(neighbor, m_width);
        Q_ASSERT(nId < m_agents.size()); // neighbor must exist
        m_edges.emplace_back(new Edge(agent(id), agent(nId), isDirected));
    }
}

SquareGrid::edges2d SquareGrid::directed4Edges(const int id, const int width)
{
    int row, col;
    Utils::ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col  ), // n
        std::make_pair(row  , col-1), // w
        std::make_pair(row  , col+1), // e
        std::make_pair(row+1, col  ), // s
    };
    return neighbors;
}

SquareGrid::edges2d SquareGrid::undirected4Edges(const int id, const int width)
{
    int row, col;
    Utils::ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col  ), // n
        std::make_pair(row  , col-1), // w
    };
    return neighbors;
}

SquareGrid::edges2d SquareGrid::directed8Edges(const int id, const int width)
{
    int row, col;
    Utils::ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col-1), // nw
        std::make_pair(row-1, col  ), // n
        std::make_pair(row-1, col+1), // ne
        std::make_pair(row  , col-1), // w
        std::make_pair(row  , col+1), // e
        std::make_pair(row+1, col-1), // sw
        std::make_pair(row+1, col  ), // s
        std::make_pair(row+1, col+1), // se
    };
    return neighbors;
}

SquareGrid::edges2d SquareGrid::undirected8Edges(const int id, const int width)
{
    int row, col;
    Utils::ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col-1), // nw
        std::make_pair(row-1, col  ), // n
        std::make_pair(row-1, col+1), // ne
        std::make_pair(row  , col-1), // w
    };
    return neighbors;
}

} // evoplex
REGISTER_GRAPH(SquareGrid)
#include "graph.moc"
