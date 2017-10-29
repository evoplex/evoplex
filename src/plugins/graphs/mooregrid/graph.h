/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MOORE_GRID_H
#define MOORE_GRID_H

#include <QPair>
#include <vector>

#include "core/agent.h"
#include "core/abstractgraph.h"

namespace evoplex {

class MooreGrid: public AbstractGraph
{
public:
    MooreGrid(const QString &name);
    bool init();
    void reset();

private:
    // graph parameters
    enum GraphAttr { Type, Height, Width };
    int m_width;
    int m_height;

    void createEdges(const int id);
    std::vector<QPair<int,int>> directedEdges(const int id);
    std::vector<QPair<int,int>> undirectedEdges(const int id);
};
}

REGISTER_GRAPH(MooreGrid)

#endif // MOORE_GRID_H
