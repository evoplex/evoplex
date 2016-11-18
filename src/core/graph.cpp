/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QtMath>

#include "core/graph.h"
#include "utils/utils.h"

Graph::Graph(QVector<IAgent*> agents, GraphType type)
    : m_valid(false)
    , m_type(type)
{
    const double width = qSqrt(agents.size());
    if (width != std::floor(width)) {
        qWarning() << "[Graph] sqrt of population ("
                   << width << ") should be integer!";
        return;
    }

    m_width = width;
    m_height = width;
    m_agents.reserve(agents.size());
    m_agents = agents;

    switch (type) {
        case MOORE_GRID:
            buildMooreGrid();
            break;
        case SQUARE_GRID:
            buildSquareGrid();
            break;
        default:
            qWarning() << "[Graph] graph type is invalid!";
            break;
    }

    m_valid = true;
}

Graph::Graph(Graph* g)
{
    // TODO: copy graph
}

Graph::~Graph()
{
    qDeleteAll(m_agents);
    m_agents.clear();
}

Graph::GraphType Graph::graphTypeFromString(QString type) const
{
    if (type == "mooreGrid") return MOORE_GRID;
    if (type == "squareGrid") return SQUARE_GRID;

    return ERROR;
}

void Graph::buildSquareGrid()
{
    const int popSize = m_agents.size();
    m_neighbours.clear();
    m_neighbours.reserve(popSize);

    // connect neighbours
    for (int id = 0; id < popSize; ++id) {
        Neighbours nbs = calcSquareNeighborhood(id);
        Q_ASSERT(nbs.size() == 4); // regular graph
        m_neighbours.append(nbs);
    }
}

void Graph::buildMooreGrid()
{
    const int popSize = m_agents.size();
    m_neighbours.clear();
    m_neighbours.reserve(popSize);

    // connect neighbours
    for (int id = 0; id < popSize; ++id) {
        Neighbours nbs = calcMooreNeighborhood(id);
        Q_ASSERT(nbs.size() == 8); // regular graph
        m_neighbours.append(nbs);
    }
}


Graph::Neighbours Graph::calcMooreNeighborhood(const int id) const
{
    int row, col;
    Utils::ind2sub(id, m_width, row, col);
    int nbs[8][2];
    // nw
    nbs[0][0] = row - 1;
    nbs[0][1] = col - 1;
    //n
    nbs[1][0] = row - 1;
    nbs[1][1] = col;
    // ne
    nbs[2][0] = row - 1;
    nbs[2][1] = col + 1;
    // w
    nbs[3][0] = row;
    nbs[3][1] = col - 1;
    // e
    nbs[4][0] = row;
    nbs[4][1] = col + 1;
    // sw
    nbs[5][0] = row + 1;
    nbs[5][1] = col - 1;
    // s
    nbs[6][0] = row + 1;
    nbs[6][1] = col;
    // se
    nbs[7][0] = row + 1;
    nbs[7][1] = col + 1;

    Neighbours neighbours;
    neighbours.reserve(8);
    for (int i = 0; i < 8; ++i) {
        if (nbs[i][0] < 0) {
            nbs[i][0] = m_height - 1;
        } else if (nbs[i][0] > m_height - 1) {
            nbs[i][0] = 0;
        }

        if (nbs[i][1] < 0) {
            nbs[i][1] = m_width - 1;
        } else if (nbs[i][1] > m_width - 1) {
            nbs[i][1] = 0;
        }

        Neighbour neighbour = m_agents.at(Utils::getLinearIdx(nbs[i][0], nbs[i][1], m_width));
        Q_ASSERT(neighbour != NULL); // must exist
        neighbours.append(neighbour);
    }

    return neighbours;
}

Graph::Neighbours Graph::calcSquareNeighborhood(const int id) const
{
    int row, col;
    Utils::ind2sub(id, m_width, row, col);
    int nbs[4][2];
    //n
    nbs[0][0] = row - 1;
    nbs[0][1] = col;
    // w
    nbs[1][0] = row;
    nbs[1][1] = col - 1;
    // e
    nbs[2][0] = row;
    nbs[2][1] = col + 1;
    // s
    nbs[3][0] = row + 1;
    nbs[3][1] = col;

    Neighbours neighbours;
    neighbours.reserve(4);
    for (int i = 0; i < 4; ++i) {
        if (nbs[i][0] < 0) {
            nbs[i][0] = m_height - 1;
        } else if (nbs[i][0] > m_height - 1) {
            nbs[i][0] = 0;
        }

        if (nbs[i][1] < 0) {
            nbs[i][1] = m_width - 1;
        } else if (nbs[i][1] > m_width - 1) {
            nbs[i][1] = 0;
        }

        Neighbour neighbour = m_agents.at(Utils::getLinearIdx(nbs[i][0], nbs[i][1], m_width));
        Q_ASSERT(neighbour != NULL); // must exist
        neighbours.append(neighbour);
    }

    return neighbours;
}
