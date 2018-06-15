/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "attributes.h"

namespace evoplex {

class Agent;
class Edge;

typedef std::vector<Edge*> Edges;

class Edge
{
public:
    explicit Edge();
    explicit Edge(Agent* origin, Agent* neighbour, Attributes* attrs, bool isDirected);
    explicit Edge(Agent* origin, Agent* neighbour, bool isDirected);
    ~Edge();

    inline const Value& attr(const char* name) const;
    inline const Value& attr(const int id) const;
    inline void setAttr(const int id, const Value& value);

    inline Agent* origin() const;
    inline Agent* neighbour() const;

private:
    Agent* m_origin;
    Agent* m_neighbour;
    Attributes* m_attrs;

    // If it's an undirected edge, it'll create a new edge in the
    // opposite direction with the same attrs.
    // This new edge is added for the sake of letting the neighbour
    // aware that an edge has been attached to him, allowing him to
    // access the attrs of this connection.
    Edge* m_undirectedEdge;
};

/************************************************************************
   Edge: Inline member functions
 ************************************************************************/

inline const Value& Edge::attr(const char* name) const
{ return m_attrs->value(name); }

inline const Value& Edge::attr(const int id) const
{ return m_attrs->value(id); }

inline void Edge::setAttr(const int id, const Value& value)
{ m_attrs->setValue(id, value); }

inline Agent* Edge::origin() const
{ return m_origin; }

inline Agent* Edge::neighbour() const
{ return m_neighbour; }


} // evoplex
#endif // EDGE_H
