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

class Node;
class BaseEdge;
using EdgePtr = std::shared_ptr<BaseEdge>;

/**
 * @brief An Edge connects a node to itself or to another node.
 * @attention An edge can only be created by an AbstractGraph derived object.
 */
class Edge
{
    friend class AbstractGraph;
    friend class TestEdge;

public:
    Edge();
    Edge(EdgePtr edge);
    Edge(const std::pair<const int, Edge>& p);

    int id() const;
    const Node& origin() const;
    const Node& neighbour() const;

    const Attributes* attrs() const;
    const Value& attr(const int id) const;
    const Value& attr(const QString& name) const;
    const Value& attr(const char* name) const;

    void setAttr(const int id, const Value& value);
    void addAttr(QString name, Value value);

private:
    EdgePtr m_ptr;
};

} // evoplex
#endif // EDGE_H
