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

#ifndef SQUARE_GRID_H
#define SQUARE_GRID_H

#include <functional>
#include <vector>

#include <plugininterface.h>

namespace evoplex {
class SquareGrid: public AbstractGraph
{
public:
    bool init() override;
    void reset() override;

private:
    bool m_periodic; // boundary conditions: false for fixed
    int m_numNeighbours;
    int m_height;
    int m_width;

    typedef std::pair<int,int> rowCol;
    typedef std::vector<rowCol> edges2d;
    typedef std::function<edges2d(const int, const int)> edgesFunc;

    // create edges with fixed boundary conditions
    void createFixedEdges(const int id, edgesFunc func);

    // create edges with periodic boundary conditions (i.e., a toroid)
    void createPeriodicEdges(const int id, edgesFunc func);

    static edges2d directed4Edges(const int id, const int width);
    static edges2d directed8Edges(const int id, const int width);

    static edges2d undirected4Edges(const int id, const int width);
    static edges2d undirected8Edges(const int id, const int width);

    // convert a linear index to row and column
    static inline void ind2sub(const int ind, const int cols, int &row, int &col);

    // return the linear index of an element in a matrix.
    static inline int linearIdx(const int row, const int col, const int cols);
    static inline int linearIdx(std::pair<int,int> rowCol, const int cols);
};

inline void SquareGrid::ind2sub(const int ind, const int cols, int &row, int &col)
{ row = ind / cols; col = ind % cols; }

inline int SquareGrid::linearIdx(const int row, const int col, const int cols)
{ return row * cols + col; }

inline int SquareGrid::linearIdx(std::pair<int,int> rowCol, const int cols)
{ return linearIdx(rowCol.first, rowCol.second, cols); }

} // evoplex
#endif // SQUARE_GRID_H
