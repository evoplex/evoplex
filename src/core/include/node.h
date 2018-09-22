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

    Node& operator=(const Node& n);
    bool operator==(const Node& n) const;
    bool operator!=(const Node& n) const;

    bool isNull() const;
    NodePtr clone() const;

    int id() const;
    float x() const;
    float y() const;

    const Attributes& attrs() const;
    const Value& attr(int id) const;
    Value attr(const QString& name, Value defaultValue=Value()) const;

    Node randNeighbour(PRG* prg) const;
    const Edges& inEdges() const;
    const Edges& outEdges() const;

    int degree() const;
    int inDegree() const;
    int outDegree() const;

    void setAttr(const int id, const Value& value);
    void setX(float x);
    void setY(float y);
    void setCoords(float x, float y);

private:
    NodePtr m_ptr;
};

} // evoplex
#endif // NODE_P_H
