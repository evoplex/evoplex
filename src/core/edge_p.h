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

#ifndef EDGE_P_H
#define EDGE_P_H

#include <memory>
#include <unordered_map>

#include "attributes.h"

namespace evoplex {

class Node;
class BaseEdge;
using EdgePtr = std::shared_ptr<BaseEdge>;

/**
 * @brief An Edge connects a node to itself or to another node.
 * @attention An edge can only be created by an AbstractGraph derived object.
 */
class BaseEdge
{
    friend class AbstractGraph;
    friend class TestEdge;

private:
    struct constructor_key { /* this is a private key accessible only to friends */ };

public:
    explicit BaseEdge(const constructor_key&, int id, const Node& origin,
        const Node& neighbour, Attributes* attrs=new Attributes(), bool ownsAttrs=true);

    ~BaseEdge();

    inline const Attributes* attrs() const;
    inline const Value& attr(const QString& name) const;
    inline const Value& attr(const char* name) const;
    inline const Value& attr(const int id) const;
    inline void setAttr(const int id, const Value& value);
    inline void addAttr(QString name, Value value);

    inline int id() const;
    inline const Node& origin() const;
    inline const Node& neighbour() const;

private:
    const int m_id;
    const Node& m_origin;
    const Node& m_neighbour;
    Attributes* m_attrs;
    const bool m_ownsAttrs;
};

/************************************************************************
   BaseEdge: Inline member functions
 ************************************************************************/

inline int BaseEdge::id() const
{ return m_id; }

inline const Node& BaseEdge::origin() const
{ return m_origin; }

inline const Node& BaseEdge::neighbour() const
{ return m_neighbour; }

inline const Attributes* BaseEdge::attrs() const
{ return m_attrs; }

inline const Value& BaseEdge::attr(const QString& name) const
{ return m_attrs->value(name); }

inline const Value& BaseEdge::attr(const char* name) const
{ return m_attrs->value(name); }

inline const Value& BaseEdge::attr(const int id) const
{ return m_attrs->value(id); }

inline void BaseEdge::setAttr(const int id, const Value& value)
{ m_attrs->setValue(id, value); }

inline void BaseEdge::addAttr(QString name, Value value)
{ m_attrs->push_back(name, value); }

} // evoplex
#endif // EDGE_H
