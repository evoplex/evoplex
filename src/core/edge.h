/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EDGE_H
#define EDGE_H

#include <QVariantHash>
#include <QVector>

namespace evoplex {

class Agent;
class Edge;

typedef QVector<Edge> Edges;  // alias: used to hold the neighbourhood of one agent

class Edge
{
public:
    explicit Edge(): m_neighbour(nullptr) {}
    explicit Edge(Agent* neighbour): m_neighbour(neighbour) {}
    explicit Edge(Agent* neighbour, const QVariantHash& attributes)
        : m_neighbour(neighbour), m_attributes(attributes) {}

    inline const QVariant getAttribute(const QString& name) const {
        return m_attributes.value(name);
    }
    inline void setAttribute(const QString& name, const QVariant& value) {
        m_attributes.insert(name, value);
    }

    inline Agent* getNeighbour() const { return m_neighbour; }
    inline void setNeighbour(Agent* neighbour) { m_neighbour = neighbour; }

private:
    Agent* m_neighbour;
    QVariantHash m_attributes;
};
}
#endif // EDGE_H
