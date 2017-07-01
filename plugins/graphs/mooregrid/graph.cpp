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

bool MooreGrid::init(const QVector<AbstractAgent>& agents, const QVariantHash& graphParams)
{
    if (!graphParams.contains("height") || !graphParams.contains("width")) {
        qWarning() << "[MooreGrid]: unable to find the parameters 'height'(int) and 'width'(int).";
        return false;
    }

    m_height = graphParams.value("height").toInt();
    m_width = graphParams.value("width").toInt();
    if (agents.size() != m_height * m_width) {
        qWarning() << "[MooreGrid]: the agent set is not compatible with the required shape."
                   << "The number of agents should be equal to 'height'*'width'.";
        return false;
    }

    m_population.clear();
    m_population.reserve(agents.size());
    for (int i = 0; i < agents.size(); ++i) {
        m_population.insert(i, agents.at(i));
    }
    return true;
}

void MooreGrid::resetNetwork()
{
    m_adjacencyList.clear();
    m_adjacencyList.reserve(m_population.size());
    // connect agents
    for (int id = 0; id < m_population.size(); ++id) {
        m_adjacencyList.insert(id, calcMooreNeighborhood(id));
    }
}

void MooreGrid::resetCoordinates()
{
    QHash<int, AbstractAgent>::iterator it = m_population.begin();
    while (it != m_population.end()) {
        int x, y;
        Utils::ind2sub(it.key(), m_width, y, x);
        it.value().setCoords(x, y);
        it.value().setId(it.key());
        ++it;
    }
}

QVariantHash MooreGrid::getGraphParams() const
{
    QVariantHash h;
    h.insert("height", m_height);
    h.insert("width", m_width);
    return h;
}

Neighbours MooreGrid::calcMooreNeighborhood(const int id) const
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

        int idx = Utils::linearIdx(nbs[i][0], nbs[i][1], m_width);
        if (!m_population.contains(idx)) {
            neighbours.push_back(Edge(m_population.value(idx)));
        } else {
            qWarning() << "[MooreNeighborhood]: the agent"
                       << idx << "does not exist!";
        }
    }
}
