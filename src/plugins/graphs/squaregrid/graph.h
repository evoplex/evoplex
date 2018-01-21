/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SQUARE_GRID_H
#define SQUARE_GRID_H

#include <QPair>
#include <vector>

#include "core/plugininterfaces.h"

namespace evoplex {
class SquareGrid: public AbstractGraph
{
public:
    SquareGrid(const QString &name);
    bool init();
    void reset();

private:
    // graph parameters
    enum GraphAttr { Height, Width };
    int m_width;
    int m_height;

    void createEdges(const int id);
    std::vector<QPair<int,int>> directedEdges(const int id);
    std::vector<QPair<int,int>> undirectedEdges(const int id);
};
}

#endif // SQUARE_GRID_H
