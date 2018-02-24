/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SQUARE_GRID_H
#define SQUARE_GRID_H

#include <functional>
#include <vector>

#include <evoplex/plugininterfaces.h>

namespace evoplex {
class SquareGrid: public AbstractGraph
{
public:
    SquareGrid(const QString &name);
    bool init();
    void reset();

private:
    // graph parameters
    enum GraphAttr { Neighbours, Height, Width };
    int m_numNeighbours;
    int m_width;
    int m_height;

    typedef std::pair<int,int> rowCol;
    typedef std::vector<rowCol> edges2d;
    typedef std::function<edges2d(const int, const int)> edgesFunc;
    void createEdges(const int id, edgesFunc func, bool isDirected);

    static edges2d directed4Edges(const int id, const int width);
    static edges2d directed8Edges(const int id, const int width);

    static edges2d undirected4Edges(const int id, const int width);
    static edges2d undirected8Edges(const int id, const int width);
};
}

#endif // SQUARE_GRID_H
