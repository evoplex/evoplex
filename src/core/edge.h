/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EDGE_H
#define EDGE_H

#include <QVariantHash>
#include <QVector>

class AbstractAgent;
class Edge;

typedef QVector<Edge> Edges;  // alias: used to hold the neighbourhood of one agent

class Edge
{
public:
    explicit Edge(): m_neighbour(nullptr) {}
    explicit Edge(AbstractAgent* neighbour): m_neighbour(neighbour) {}
    explicit Edge(AbstractAgent* neighbour, const QVariantHash& attributes)
        : m_neighbour(neighbour), m_attributes(attributes) {}

    inline const QVariant getAttribute(const QString& name) const {
        return m_attributes.value(name);
    }
    inline void setAttribute(const QString& name, const QVariant& value) {
        m_attributes.insert(name, value);
    }

    inline AbstractAgent* getNeighbour() const { return m_neighbour; }
    inline void setNeighbour(AbstractAgent* neighbour) { m_neighbour = neighbour; }

private:
    AbstractAgent* m_neighbour;
    QVariantHash m_attributes;
};

#endif // EDGE_H
