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

#include "include/node.h"
#include "node_p.h"

namespace evoplex {

Node::Node()
    : m_ptr(nullptr)
{}

Node::Node(NodePtr node)
    : m_ptr(node)
{}

Node::Node(const std::pair<const int, Node>& p)
    : m_ptr(p.second.m_ptr)
{}

Node::Node(const std::pair<const int, Edge>& p)
    : m_ptr(p.second.neighbour().m_ptr)
{}

Node& Node::operator=(const Node& n)
{
    if (this != &n) { // check for self-assignment
        m_ptr = n.m_ptr;
    }
    return *this;
}

bool Node::operator==(const Node& n) const
{ return m_ptr == n.m_ptr; }

bool Node::operator!=(const Node& n) const
{ return m_ptr != n.m_ptr; }

bool Node::isNull() const
{ return m_ptr ? false : true; }

NodePtr Node::clone() const
{ return m_ptr->clone(); }

int Node::id() const
{ return m_ptr->id(); }

float Node::x() const
{ return m_ptr->x(); }

float Node::y() const
{ return m_ptr->y(); }

const Attributes& Node::attrs() const
{ return m_ptr->attrs(); }

const Value& Node::attr(int id) const
{ return m_ptr->attr(id); }

Value Node::attr(const QString& name, Value defaultValue) const
{ return m_ptr->attr(name, defaultValue); }

Node Node::randNeighbour(PRG* prg) const
{ return m_ptr->randNeighbour(prg); }

const Edges& Node::inEdges() const
{ return m_ptr->inEdges(); }

const Edges& Node::outEdges() const
{ return m_ptr->outEdges(); }

int Node::degree() const
{ return m_ptr->degree(); }

int Node::inDegree() const
{ return m_ptr->inDegree(); }

int Node::outDegree() const
{ return m_ptr->outDegree(); }

void Node::setAttr(const int id, const Value& value)
{ m_ptr->setAttr(id, value); }

void Node::setX(float x)
{ m_ptr->setX(x); }

void Node::setY(float y)
{ m_ptr->setY(y); }

void Node::setCoords(float x, float y)
{ m_ptr->setCoords(x, y); }

} // evoplex
