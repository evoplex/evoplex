/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QtMath>

#include "../src/utils/utils.h"
#include "mooregrid.h"

MooreGrid::MooreGrid(const QString& name)
    : AbstractGraph(name)
    , m_height(0)
    , m_width(0)
{
}

bool MooreGrid::init(QVector<AbstractAgent*> agents, QVariantHash graphParams)
{
    if (!graphParams.contains("height") || !graphParams.contains("width")) {
        qWarning() << "[MooreGrid]: unable to find the parameters 'height'(int) and 'width'(int).";
        return false;
    }

    m_height = graphParams.value("height");
    m_width = graphParams.value("width");
    if (agents.size() != m_height * m_width) {
        qWarning() << "[MooreGrid]: the agent set is not compatible with the required shape."
                   << "The number of agents should be equal to 'height'*'width'.";
        return false;
    }

    m_agents = agents;
    return true;
}

void MooreGrid::resetLinks()
{
    m_links.clear();
    m_links.reserve(m_agents.size());
    // connect agents
    for (int id = 0; id < m_agents.size(); ++id) {
        QVector<AbstractAgent*> nbs = calcMooreNeighborhood(id);
        Q_ASSERT(nbs.size() == 8); // regular graph
        m_links.insert(id, nbs);
    }
}

bool MooreGrid::buildCoordinates()
{
    int id = 0;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            AbstractAgent* a = m_agents.at(id);
            a->setProperty("id", id);
            a->setProperty("x", x);
            a->setProperty("y", y);
            ++id;
        }
    }
    return true;
}

QVariantHash MooreGrid::getGraphParams() const
{
    QVariantHash h;
    h.insert("height", m_height);
    h.insert("width", m_width);
    return h;
}

QVector<AbstractAgent*> MooreGrid::calcMooreNeighborhood(const int id) const
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

    QVector<AbstractAgent*> neighbours;
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

        QVector<AbstractAgent*> neighbour = m_agents.at(Utils::getLinearIdx(nbs[i][0], nbs[i][1], m_width));
        Q_ASSERT(neighbour != NULL); // must exist
        neighbours.append(neighbour);
    }

    return neighbours;
}
