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

class MooreGrid: public AbstractGraph
{
public:
    MooreGrid(const QString &name);

    bool init(QVector<AbstractAgent*> agents, const QVariantHash& graphParams);
    void resetNetwork();
    bool buildCoordinates();
    QVariantHash getGraphParams() const;

private:
    // graph parameters
    int m_width;
    int m_height;

    void calcMooreNeighborhood(const int id, Neighbours &neighbours) const;
};

REGISTER_GRAPH(MooreGrid)

#endif // MOORE_GRID_H
