/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtDebug>
#include <QtMath>

#include "plugin.h"

namespace evoplex {

bool SquareGrid::init()
{
    if (!attrExists("boundary") || !attrExists("neighbours") ||
        !attrExists("height") || !attrExists("width")) {
        qWarning() << "missing attributes.";
        return false;
    }

    m_periodic = attr("boundary").toQString() == QString("periodic");
    m_numNeighbours = attr("neighbours").toInt();
    m_height = attr("height").toInt();
    m_width = attr("width").toInt();

    if (numNodes() != m_height * m_width) {
        qWarning() << "Wrong shape! The number of nodes should be equal to 'height'*'width'.";
        return false;
    }

    return true;
}

void SquareGrid::reset()
{
    m_edges.clear();

    edgesFunc func;
    if (m_numNeighbours == 4) {
        func = isDirected() ? directed4Edges : undirected4Edges;
    } else {
        func = isDirected() ? directed8Edges : undirected8Edges;
    }

    if (m_periodic) {
        for (auto const& node : m_nodes) {
            int x, y;
            ind2sub(node.first, m_width, y, x);
            node.second->setCoords(x, y);
            createPeriodicEdges(node.first, func);
        }
    } else {
        for (auto const& node : m_nodes) {
            int x, y;
            ind2sub(node.first, m_width, y, x);
            node.second->setCoords(x, y);
            createFixedEdges(node.first, func);
        }
    }
}

void SquareGrid::createPeriodicEdges(const int id, const edgesFunc& func)
{
    edges2d neighbors = func(id, m_width);
    for (std::pair<int,int> neighbor : neighbors) {
        if (neighbor.first < 0) {
            neighbor.first = m_height - 1;
        } else if (neighbor.first >= m_height) {
            neighbor.first = 0;
        }

        if (neighbor.second < 0) {
            neighbor.second = m_width - 1;
        } else if (neighbor.second >= m_width) {
            neighbor.second = 0;
        }

        int nId = linearIdx(neighbor, m_width);
        Q_ASSERT_X(nId < numNodes(), "SquareGrid::createEdges", "neighbor must exist");
        addEdge(id, nId, new Attributes());
    }
}

void SquareGrid::createFixedEdges(const int id, const edgesFunc& func)
{
    edges2d neighbors = func(id, m_width);
    for (std::pair<int,int> neighbor : neighbors) {
        if (neighbor.first < 0 || neighbor.first >= m_height ||
            neighbor.second < 0 || neighbor.second >= m_width) {
            continue;
        }
        int nId = linearIdx(neighbor, m_width);
        Q_ASSERT_X(nId < numNodes(), "SquareGrid::createEdges", "neighbor must exist");
        addEdge(id, nId, new Attributes());
    }
}

SquareGrid::edges2d SquareGrid::directed4Edges(const int id, const int width)
{
    int row, col;
    ind2sub(id, width, row, col);
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
    ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col  ), // n
        std::make_pair(row  , col-1), // w
    };
    return neighbors;
}

SquareGrid::edges2d SquareGrid::directed8Edges(const int id, const int width)
{
    int row, col;
    ind2sub(id, width, row, col);
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
    ind2sub(id, width, row, col);
    edges2d neighbors {
        std::make_pair(row-1, col-1), // nw
        std::make_pair(row-1, col  ), // n
        std::make_pair(row-1, col+1), // ne
        std::make_pair(row  , col-1), // w
    };
    return neighbors;
}

} // evoplex
REGISTER_PLUGIN(SquareGrid)
#include "plugin.moc"
