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

    bool init(const QVector<AbstractAgent> &agents, const QVariantHash& graphParams);
    void resetNetwork();
    void resetCoordinates();
    QVariantHash getGraphParams() const;

private:
    // graph parameters
    int m_width;
    int m_height;

    Neighbours calcMooreNeighborhood(const int id) const;
};

REGISTER_GRAPH(MooreGrid)

#endif // MOORE_GRID_H
