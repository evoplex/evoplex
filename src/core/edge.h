/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "attributes.h"
#include "enums.h"

namespace evoplex {

class Agent;
class Edge;

typedef std::vector<Edge*> Edges;

class Edge
{
public:
    explicit Edge();
    explicit Edge(Agent* origin, Agent* neighbour, Attributes* attrs, bool isDirected);
    explicit Edge(Agent* origin, Agent* neighbour, bool isDirected)
        : Edge(origin, neighbour, new Attributes(), isDirected) {}
    explicit Edge(Agent* origin, Agent* neighbour)
        : Edge(origin, neighbour, new Attributes(), true) {}
    ~Edge();

    inline const Value& attribute(const char* name) const { return m_attrs->value(name); }
    inline const Value& attribute(const int id) const { return m_attrs->value(id); }
    inline void setAttribute(const int id, const Value& value) { m_attrs->setValue(id, value); }
    inline Agent* getNeighbour() const { return m_neighbour; }

private:
    Agent* m_origin;
    Agent* m_neighbour;
    Attributes* m_attrs;
    bool m_isDirected;
};
}
#endif // EDGE_H
