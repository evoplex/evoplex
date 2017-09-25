/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_AGENT_H
#define ABSTRACT_AGENT_H

#include "attributes.h"
#include "edge.h"
#include "prg.h"

namespace evoplex {

typedef QVector<Agent*> Agents;

class Agent
{
public:
    explicit Agent() : Agent(Attributes()) {}
    explicit Agent(Attributes attr)
        : m_attributes(attr), m_edges(nullptr), m_x(0), m_y(0) {}

    ~Agent() {}

    inline Agent* clone() { return new Agent(m_attributes); }

    inline const Value& attribute(const char* name) const { return m_attributes.value(name); }
    inline const Value& attribute(const int id) const { return m_attributes.value(id); }
    inline void setAttribute(const int id, const Value& value) { m_attributes.setValue(id, value); }

    inline const int getX() const { return m_x; }
    inline void setX(int x) { m_x = x; }
    inline const int getY() const { return m_y; }
    inline void setY(int y) { m_y = y; }
    inline void setCoords(int x, int y) { setX(x); setY(y); }

    inline const Edges* getEdges() const { return m_edges; }
    inline void setEdges(Edges* edges) { m_edges = edges; }
    inline Agent* getNeighbour(int localId) const { return m_edges->at(localId).getNeighbour(); }
    inline Agent* getRandomNeighbour(PRG* prg) const {
        return m_edges->at(prg->randI(m_edges->size())).getNeighbour();
    }

private:
    int m_x;
    int m_y;
    Attributes m_attributes;
    Edges* m_edges; // pointer owned by the graph
};
}

#endif // ABSTRACT_AGENT_H
