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
    inline const Value& attr(int id) const;
    inline Value attr(const QString& name, Value defaultValue=Value()) const;
    inline void setAttr(int id, const Value& value);
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

inline const Value& BaseEdge::attr(int id) const
{ return m_attrs->value(id); }

inline Value BaseEdge::attr(const QString& name, Value defaultValue) const
{ return m_attrs->value(name, defaultValue); }

inline void BaseEdge::setAttr(int id, const Value& value)
{ m_attrs->setValue(id, value); }

inline void BaseEdge::addAttr(QString name, Value value)
{ m_attrs->push_back(name, value); }

} // evoplex
#endif // EDGE_H
