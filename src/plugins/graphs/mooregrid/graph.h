/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MOORE_GRID_H
#define MOORE_GRID_H

#include <QObject>
#include <QVariantHash>
#include <QVector>

#include "core/abstractagent.h"
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
    enum GraphAttr { Height, Width };
    int m_width;
    int m_height;

    Edges* createEdges(const int id) const;
};
}

REGISTER_GRAPH(MooreGrid)

#endif // MOORE_GRID_H
