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

#include "node_p.h"
#include "node.h"

// we need this cpp file to avoi weak-vtable issues
namespace evoplex {

BaseNode::BaseNode(const constructor_key&, int id, const Attributes& attrs, float x, float y)
    : m_id(id),
      m_attrs(attrs),
      m_x(x),
      m_y(y)
{
}

BaseNode::BaseNode(const constructor_key& k, int id, const Attributes& attr)
    : BaseNode(k, id, attr, 0, id) {}

BaseNode::~BaseNode()
{
}

Node BaseNode::randNeighbour(PRG* prg) const
{
    if (m_outEdges.empty()) {
        return Node();
    }
    auto i = prg->uniform(outDegree()-1);
    return std::next(m_outEdges.cbegin(), i)->second.neighbour();
}

/*******************/

UNode::UNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y)
    : BaseNode(k, id, attrs, x, y)
{
}

UNode::UNode(const constructor_key& k, int id, const Attributes& attrs)
    : BaseNode(k, id, attrs)
{
}

/*******************/

DNode::DNode(const constructor_key& k, int id, const Attributes& attrs, float x, float y)
    : BaseNode(k, id, attrs, x, y)
{
}

DNode::DNode(const constructor_key& k, int id, const Attributes& attrs)
    : BaseNode(k, id, attrs)
{
}

} // evoplex
