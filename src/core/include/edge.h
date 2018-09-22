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
    const Value& attr(int id) const;
    Value attr(const QString& name, Value defaultValue=Value()) const;

    void setAttr(const int id, const Value& value);
    void addAttr(QString name, Value value);

private:
    EdgePtr m_ptr;
};

} // evoplex
#endif // EDGE_H
