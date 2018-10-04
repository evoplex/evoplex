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

/**
 * @brief It wraps a std::shared_ptr<BaseNode>.
 * @ingroup PublicAPI
 */
class Node
{
    friend class AbstractGraph;
    friend class NodesPrivate;
    friend class TestNodes;

public:
    /**
     * @brief Constructor.
     * @param node std::shared_ptr<BaseNode>.
     */
    Node(NodePtr node);
    /**
     * @brief Constructor to ease range-based for loops.
     * @param p A pair <nodeId, Node>.
     */
    Node(const std::pair<const int, Node>& p);
    /**
     * @brief Constructor to ease range-based for loops.
     * @param p A pair <nodeId, Edge>.
     */
    Node(const std::pair<const int, Edge>& p);
    //! Constructor.
    Node();

    /**
     * @brief Sets the current node to @p n.
     */
    Node& operator=(const Node& n);

    /**
     * @brief Checks if @p n and the current Node point to the same BaseNode.
     */
    bool operator==(const Node& n) const;

    /**
     * @brief Checks if @p n and the current Node point to the same BaseNode.
     */
    bool operator!=(const Node& n) const;

    /**
     * @brief Checks if the current Node is null.
     */
    bool isNull() const;

    //! @copydoc BaseNode::clone
    NodePtr clone() const;
    //! @copydoc BaseNode::id
    int id() const;
    //! @copydoc BaseNode::x
    float x() const;
    //! @copydoc BaseNode::y
    float y() const;

    //! @copydoc BaseNode::attrs
    const Attributes& attrs() const;
    //! @copydoc BaseNode::attr
    const Value& attr(int id) const;
    //! @copydoc BaseNode::attr(const QString& name, Value defaultValue=Value()) const
    Value attr(const QString& name, Value defaultValue=Value()) const;

    //! @copydoc BaseNode::randNeighbour
    Node randNeighbour(PRG* prg) const;
    //! @copydoc BaseNode::inEdges
    const Edges& inEdges() const;
    //! @copydoc BaseNode::outEdges
    const Edges& outEdges() const;

    //! @copydoc BaseNode::degree
    int degree() const;
    //! @copydoc BaseNode::inDegree
    int inDegree() const;
    //! @copydoc BaseNode::outDegree
    int outDegree() const;

    //! @copydoc BaseNode::setAttr
    void setAttr(const int id, const Value& value);
    //! @copydoc BaseNode::setX
    void setX(float x);
    //! @copydoc BaseNode::setY
    void setY(float y);
    //! @copydoc BaseNode::setCoords
    void setCoords(float x, float y);

private:
    NodePtr m_ptr;
};

} // evoplex
#endif // NODE_P_H
