/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#include "include/edge.h"
#include "edge_p.h"

namespace evoplex {

BaseEdge::BaseEdge(const constructor_key&, int id, const Node& origin,
                   const Node& neighbour, Attributes* attrs, bool ownsAttrs)
    : m_id(id),
      m_origin(origin),
      m_neighbour(neighbour),
      m_attrs(attrs),
      m_ownsAttrs(ownsAttrs)
{
}

BaseEdge::~BaseEdge()
{
    if (m_ownsAttrs) {
        delete m_attrs;
    }
}

Edge::Edge()
    : m_ptr(nullptr)
{}

Edge::Edge(EdgePtr edge)
    : m_ptr(edge)
{}

Edge::Edge(const std::pair<const int, Edge>& p)
    : m_ptr(p.second.m_ptr)
{}

int Edge::id() const
{ return m_ptr->id(); }

const Node& Edge::origin() const
{ return m_ptr->origin(); }

const Node& Edge::neighbour() const
{ return m_ptr->neighbour(); }

const Attributes* Edge::attrs() const
{ return m_ptr->attrs(); }

const Value& Edge::attr(int id) const
{ return m_ptr->attr(id); }

Value Edge::attr(const QString& name, Value defaultValue) const
{ return m_ptr->attr(name, defaultValue); }

void Edge::setAttr(const int id, const Value& value)
{ m_ptr->setAttr(id, value); }

void Edge::addAttr(QString name, Value value)
{ m_ptr->addAttr(name, value); }

} // evoplex
