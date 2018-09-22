/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
