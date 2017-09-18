/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QtMath>

#include "graph.h"
#include "utils/utils.h"

MooreGrid::MooreGrid(const QString& name)
    : AbstractGraph(name)
    , m_height(0)
    , m_width(0)
{
}

bool MooreGrid::init(const QVariantHash& graphParams)
{
    if (!graphParams.contains("height") || !graphParams.contains("width")) {
        qWarning() << "[MooreGrid]: unable to find the parameters 'height'(int) and 'width'(int).";
        return false;
    }

    m_height = graphParams.value("height").toInt();
    m_width = graphParams.value("width").toInt();
    if (getAgents().size() != m_height * m_width) {
        qWarning() << "[MooreGrid]: the agent set is not compatible with the required shape."
                   << "The number of agents should be equal to 'height'*'width'.";
        return false;
    }

    reset();

    return true;
}

void MooreGrid::reset()
{
    AdjacencyList adjacencyList;
    adjacencyList.reserve(getAgents().size());

    for (int id = 0; id < getAgents().size(); ++id) {
        AbstractAgent* agent = getAgent(id);

        int x, y;
        Utils::ind2sub(id, m_width, y, x);
        agent->setCoords(x, y);

        Edges* edges = createEdges(id);
        agent->setEdges(edges);
        adjacencyList.insert(agent, edges);
    }
    setAdjacencyList(adjacencyList);
}

QVariantHash MooreGrid::getGraphParams() const
{
    QVariantHash h;
    h.insert("height", m_height);
    h.insert("width", m_width);
    return h;
}

Edges* MooreGrid::createEdges(const int id) const
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

    Edges* edges = new Edges();
    edges->reserve(8);
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

        int idx = Utils::linearIdx(nbs[i][0], nbs[i][1], m_width);
        if (idx < getAgents().size()) {
            edges->push_back(Edge(getAgent(idx)));
        } else {
            qWarning() << "[MooreNeighborhood]: the agent"
                       << idx << "does not exist!";
        }
    }
    return edges;
}
