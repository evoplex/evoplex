/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EDGE_H
#define EDGE_H

#include <QVector>

#include "attributes.h"

namespace evoplex {

class Agent;
class Edge;

typedef QVector<Edge> Edges;  // alias: used to hold the neighbourhood of one agent

class Edge
{
public:
    explicit Edge(): m_neighbour(nullptr) {}
    explicit Edge(Agent* neighbour): m_neighbour(neighbour) {}
    explicit Edge(Agent* neighbour, Attributes attrs)
        : m_neighbour(neighbour), m_attributes(attrs) {}

    inline const Value& attribute(const char* name) const { return m_attributes.value(name); }
    inline const Value& attribute(const int id) const { return m_attributes.value(id); }
    inline void setAttribute(const int id, const Value& value) { m_attributes.setValue(id, value); }

    inline Agent* getNeighbour() const { return m_neighbour; }
    inline void setNeighbour(Agent* neighbour) { m_neighbour = neighbour; }

private:
    Agent* m_neighbour;
    Attributes m_attributes;
};
}
#endif // EDGE_H
