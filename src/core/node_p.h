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

#ifndef NODE_P_H
#define NODE_P_H

#include <memory>

#include "attributes.h"
#include "edges.h"
#include "prg.h"

namespace evoplex {

class BaseNode;
using NodePtr = std::shared_ptr<BaseNode>;

/**
 * @brief A common interface for the node's classes.
 * @see BaseNode, UNode, DNode
 */
class NodeInterface
{
    friend class AbstractGraph;
    friend class Nodes;
    friend class TestNode;
    friend class TestEdge;

public:
    //! Destructor.
    virtual ~NodeInterface() = default;

    /**
     * @brief Creates a new std::shared_ptr<BaseNode> with the
     *        same data of the current Node.
     */
    virtual NodePtr clone() const = 0;

    /**
     * @brief Gets the edges entering the node.
     */
    virtual const Edges& inEdges() const = 0;

    /**
     * @brief Gets the edges leaving the node.
     */
    virtual const Edges& outEdges() const = 0;

    /**
     * @brief Gets the node's degree.
     */
    virtual int degree() const = 0;

    /**
     * @brief Gets the node's in-degree, i.e.,
     *        the number of edges entering the node.
     */
    virtual int inDegree() const = 0;

    /**
     * @brief Gets the node's out-degree, i.e.,
     *        the number of edges leaving the node.
     */
    virtual int outDegree() const = 0;

private:
    virtual void addInEdge(const Edge& inEdge) = 0;
    virtual void addOutEdge(const Edge& outEdge) = 0;
    virtual void removeInEdge(const int edgeId) = 0;
    virtual void removeOutEdge(const int edgeId) = 0;
    virtual void clearInEdges() = 0;
    virtual void clearOutEdges() = 0;
};

/**
 * @brief A node belongs to either a directed or undirected graph.
 *        Directed graphs are composed of DNode objects, while an undirected
 *        graph is composed of UNode objects.
 *
 * @attention A node can only be created by an AbstractGraph derived object
 *            or from a Nodes container.
 */
class BaseNode : public NodeInterface
{
    friend class AbstractGraph;
    friend class NodesPrivate;
    friend class TestNode;
    friend class TestEdge;

public:
    /**
     * @brief Gets all the node's Attributes.
     */
    inline const Attributes& attrs() const;
    //! @copydoc Attributes::value
    inline const Value& attr(int id) const;
    //! @copydoc Attributes::value(const QString& name, Value defaultValue=Value()) const
    inline Value attr(const QString& name, Value defaultValue=Value()) const;
    //! @copydoc Attributes::setValue
    inline void setAttr(int id, const Value& value);

    /**
     * @brief Gets the node's id.
     */
    inline int id() const;
    /**
     * @brief Gets the node's x coordinate.
     */
    inline float x() const;
    /**
     * @brief Gets the node's y coordinate.
     */
    inline float y() const;

    /**
     * @brief Sets the node's @p x coordinate.
     */
    inline void setX(float x);
    /**
     * @brief Sets the node's @p y coordinate.
     */
    inline void setY(float y);
    /**
     * @brief Sets the node's coordinates.
     */
    inline void setCoords(float x, float y);

    /**
     * @brief Gets a random neighbour.
     * Returns an invalid/empty Node if the node has no neighbours.
     */
    Node randNeighbour(PRG* prg) const;

protected:
    Edges m_outEdges;

    /**
     * @brief This is a private key accessible only to friend classes.
     */
    struct constructor_key { };

    explicit BaseNode(const constructor_key&, int id, const Attributes& attrs, float x, float y);
    explicit BaseNode(const constructor_key& k, int id, const Attributes& attr);
    ~BaseNode() override;

private:
    const int m_id;
    Attributes m_attrs;
    float m_x;
    float m_y;
};

/**
 * @brief BaseNode implementation for undirected nodes.
 * @see BaseNode
 */
class UNode : public BaseNode
{
public:
    explicit UNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y);
    explicit UNode(const constructor_key& k, int id, const Attributes& attrs);
    ~UNode() override = default;

    inline NodePtr clone() const override;
    inline const Edges& inEdges() const override;
    inline const Edges& outEdges() const override;
    inline int degree() const override;
    inline int inDegree() const override;
    inline int outDegree() const override;

private:
    inline void addInEdge(const Edge& inEdge) override;
    inline void addOutEdge(const Edge& outEdge) override;
    inline void removeInEdge(const int edgeId) override;
    inline void removeOutEdge(const int edgeId) override;
    inline void clearInEdges() override;
    inline void clearOutEdges() override;
};

/**
 * @brief BaseNode implementation for directed nodes.
 * @see BaseNode
 */
class DNode : public BaseNode
{
public:
    explicit DNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y);
    explicit DNode(const constructor_key& k, int id, const Attributes& attrs);
    ~DNode() override = default;

    inline NodePtr clone() const override;
    inline const Edges& inEdges() const override;
    inline const Edges& outEdges() const override;
    inline int degree() const override;
    inline int inDegree() const override;
    inline int outDegree() const override;

private:
    Edges m_inEdges;

    inline void addInEdge(const Edge& inEdge) override;
    inline void addOutEdge(const Edge& outEdge) override;
    inline void removeInEdge(const int edgeId) override;
    inline void removeOutEdge(const int edgeId) override;
    inline void clearInEdges() override;
    inline void clearOutEdges() override;
};

/************************************************************************
   BaseNode: Inline member functions
 ************************************************************************/

inline const Attributes& BaseNode::attrs() const
{ return m_attrs; }

inline const Value& BaseNode::attr(int id) const
{ return m_attrs.value(id); }

inline Value BaseNode::attr(const QString& name, Value defaultValue) const
{ return m_attrs.value(name, defaultValue); }

inline void BaseNode::setAttr(int id, const Value& value)
{ m_attrs.setValue(id, value); }

inline int BaseNode::id() const
{ return m_id; }

inline float BaseNode::x() const
{ return m_x; }

inline void BaseNode::setX(float x)
{ m_x = x; }

inline float BaseNode::y() const
{ return m_y; }

inline void BaseNode::setY(float y)
{ m_y = y; }

inline void BaseNode::setCoords(float x, float y)
{ setX(x); setY(y); }

/************************************************************************
   UNode: Inline member functions
 ************************************************************************/

inline NodePtr UNode::clone() const
{ return std::make_shared<UNode>(constructor_key(), id(), attrs(), x(), y()); }

inline const Edges& UNode::inEdges() const
{ return m_outEdges; }

inline const Edges& UNode::outEdges() const
{ return m_outEdges; }

inline int UNode::degree() const
{ return static_cast<int>(m_outEdges.size()); }

inline int UNode::inDegree() const
{ return degree(); }

inline int UNode::outDegree() const
{ return degree(); }

inline void UNode::addInEdge(const Edge& inEdge)
{ addOutEdge(inEdge); }

inline void UNode::addOutEdge(const Edge& outEdge)
{ m_outEdges.insert({outEdge.id(), outEdge}); }

inline void UNode::removeInEdge(const int edgeId)
{ removeOutEdge(edgeId); }

inline void UNode::removeOutEdge(const int edgeId)
{ m_outEdges.erase(edgeId); }

inline void UNode::clearInEdges()
{ clearOutEdges(); }

inline void UNode::clearOutEdges()
{ m_outEdges.clear(); }

/************************************************************************
   DNode: Inline member functions
 ************************************************************************/

NodePtr DNode::clone() const
{ return std::make_shared<DNode>(constructor_key(), id(), attrs(), x(), y()); }

inline const Edges& DNode::inEdges() const
{ return m_inEdges; }

inline const Edges& DNode::outEdges() const
{ return m_outEdges; }

inline int DNode::degree() const
{ return inDegree() + outDegree(); }

inline int DNode::inDegree() const
{ return static_cast<int>(m_inEdges.size()); }

inline int DNode::outDegree() const
{ return static_cast<int>(m_outEdges.size()); }

inline void DNode::addInEdge(const Edge& inEdge)
{ m_inEdges.insert({inEdge.id(), inEdge}); }

inline void DNode::addOutEdge(const Edge& outEdge)
{ m_outEdges.insert({outEdge.id(), outEdge}); }

inline void DNode::removeInEdge(const int edgeId)
{ m_inEdges.erase(edgeId); }

inline void DNode::removeOutEdge(const int edgeId)
{ m_outEdges.erase(edgeId); }

inline void DNode::clearInEdges()
{ m_inEdges.clear(); }

inline void DNode::clearOutEdges()
{ m_outEdges.clear(); }

} // evoplex
#endif // NODE_P_H
