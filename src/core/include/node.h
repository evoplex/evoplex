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

#ifndef NODE_H
#define NODE_H

#include <memory>
#include <unordered_map>

#include "attributes.h"
#include "edges.h"

namespace evoplex {

class BaseNode;
using NodePtr = std::shared_ptr<BaseNode>;

class Node
{
    friend class AbstractGraph;
    friend class NodesPrivate;
    friend class TestNodes;

public:
    Node();
    Node(NodePtr node);
    Node(const std::pair<const int, Node>& p);
    Node(const std::pair<const int, Edge>& p);

    bool isNull() const;
    NodePtr clone() const;

    int id() const;
    int x() const;
    int y() const;

    const Attributes& attrs() const;
    const Value& attr(const int& id) const;
    const Value& attr(const QString& name) const;

    const Node& randNeighbour(PRG* prg) const;
    const Edges& inEdges() const;
    const Edges& outEdges() const;

    int degree() const;
    int inDegree() const;
    int outDegree() const;

    void setAttr(const int id, const Value& value);
    void setX(int x);
    void setY(int y);
    void setCoords(int x, int y);

private:
    NodePtr m_ptr;
};

} // evoplex
#endif // NODE_P_H
