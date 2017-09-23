/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_AGENT_H
#define ABSTRACT_AGENT_H

#include "core/attributes.h"
#include "core/edge.h"
#include "utils/prg.h"

namespace evoplex {

typedef QVector<AbstractAgent*> Agents;

class AbstractAgent
{
public:
    explicit AbstractAgent() : AbstractAgent(Attributes()) {}
    explicit AbstractAgent(Attributes attr)
        : m_attributes(attr), m_edges(nullptr), m_x(0), m_y(0) {}

    ~AbstractAgent() {
        delete m_edges;
        m_edges = nullptr;
    }

    inline AbstractAgent* clone() { return new AbstractAgent(m_attributes); }

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
    inline AbstractAgent* getNeighbour(int localId) const { return m_edges->at(localId).getNeighbour(); }
    inline AbstractAgent* getRandomNeighbour(PRG* prg) const {
        return m_edges->at(prg->randI(m_edges->size())).getNeighbour();
    }

private:
    int m_x;
    int m_y;
    Attributes m_attributes;
    Edges* m_edges;
};
}

#endif // ABSTRACT_AGENT_H
