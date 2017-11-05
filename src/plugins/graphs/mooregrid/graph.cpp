/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QtMath>

#include "graph.h"
#include "core/utils.h"

namespace evoplex {

MooreGrid::MooreGrid(const QString& name)
    : AbstractGraph(name)
    , m_height(0)
    , m_width(0)
{
}

bool MooreGrid::init()
{
    m_height = attrs()->value(Height).toInt;
    m_width = attrs()->value(Width).toInt;
    if (agents().size() != m_height * m_width) {
        qWarning() << "[MooreGrid]: the agent set is not compatible with the required shape."
                   << "The number of agents should be equal to 'height'*'width'.";
        return false;
    }
    return true;
}

void MooreGrid::reset()
{
    qDeleteAll(m_edges);
    m_edges.clear();
    Edges().swap(m_edges);

    for (int id = 0; id < m_agents.size(); ++id) {
        int x, y;
        Utils::ind2sub(id, m_width, y, x);
        m_agents.at(id)->setCoords(x, y);
        createEdges(id);
    }
}

void MooreGrid::createEdges(const int id)
{
    bool directed;
    std::vector<QPair<int,int>> neighbors;
    if (type() == Directed) {
        neighbors = directedEdges(id);
        directed = true;
    } else {
        neighbors = undirectedEdges(id);
        directed = false;
    }

    for (QPair<int,int> neighbor : neighbors) {
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
        m_edges.emplace_back(new Edge(agent(id), agent(nId), directed));
    }
}

std::vector<QPair<int,int>> MooreGrid::directedEdges(const int id)
{
    int row, col;
    Utils::ind2sub(id, m_width, row, col);
    std::vector<QPair<int,int>> neighbors {
        qMakePair(row-1, col-1), // nw
        qMakePair(row-1, col  ), // n
        qMakePair(row-1, col+1), // ne
        qMakePair(row  , col-1), // w
        qMakePair(row  , col+1), // e
        qMakePair(row+1, col-1), // sw
        qMakePair(row+1, col  ), // s
        qMakePair(row+1, col+1), // se
    };
    return neighbors;
}

std::vector<QPair<int,int>> MooreGrid::undirectedEdges(const int id)
{
    int row, col;
    Utils::ind2sub(id, m_width, row, col);
    std::vector<QPair<int,int>> neighbors {
        qMakePair(row-1, col-1), // nw
        qMakePair(row-1, col  ), // n
        qMakePair(row-1, col+1), // ne
        qMakePair(row  , col-1), // w
    };
    return neighbors;
}

}
