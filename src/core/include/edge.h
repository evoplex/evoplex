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

#include <memory>
#include <unordered_map>

#include "attributes.h"

namespace evoplex {

class Edge;
class Node;

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<Edge> EdgePtr;

class Edge
{
public:
    explicit Edge(int id, const NodePtr& origin, const NodePtr& neighbour, Attributes* attrs, bool takesOwnership)
        : m_id(id), m_origin(origin), m_neighbour(neighbour), m_attrs(attrs), m_takesOwnership(takesOwnership) {}

    ~Edge() { if (m_takesOwnership) delete m_attrs; }

    inline const Attributes* attrs() const;
    inline const Value& attr(const char* name) const;
    inline const Value& attr(const int id) const;
    inline void setAttr(const int id, const Value& value);

    inline int id() const;
    inline const NodePtr& origin() const;
    inline const NodePtr& neighbour() const;

private:
    const int m_id;
    const NodePtr& m_origin;
    const NodePtr& m_neighbour;
    Attributes* m_attrs;
    bool m_takesOwnership;
};

struct Edges : public std::unordered_map<int, EdgePtr>
{
    struct Iterator : public iterator {
        Iterator() {}
        Iterator(const iterator& _a) : iterator(_a) {}
        const int& id() const { return (*this)->first; }
        EdgePtr& edge() const { return (*this)->second; }
    };

    struct ConstIterator : public const_iterator {
        ConstIterator() {}
        ConstIterator(const const_iterator& _a) : const_iterator(_a) {}
        const int& id() const { return (*this)->first; }
        const EdgePtr& edge() const { return (*this)->second; }
    };

    struct Pair {
        const std::pair<const int, EdgePtr>& _p;
        Pair(const std::pair<const int, EdgePtr>& _a) : _p(_a) {}
        const int& id() const { return _p.first; }
        const EdgePtr& edge() const { return _p.second; }
    };
};

/************************************************************************
   Edge: Inline member functions
 ************************************************************************/

inline const Attributes* Edge::attrs() const
{ return m_attrs; }

inline const Value& Edge::attr(const char* name) const
{ return m_attrs->value(name); }

inline const Value& Edge::attr(const int id) const
{ return m_attrs->value(id); }

inline void Edge::setAttr(const int id, const Value& value)
{ m_attrs->setValue(id, value); }

inline int Edge::id() const
{ return m_id; }

inline const NodePtr& Edge::origin() const
{ return m_origin; }

inline const NodePtr& Edge::neighbour() const
{ return m_neighbour; }


} // evoplex
#endif // EDGE_H
