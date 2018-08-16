/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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
    const int od = outDegree();
    if (od < 1) {
        return Node();
    } else if (od == 1) {
        return m_outEdges.begin()->second.neighbour();
    }
    return std::next(m_outEdges.cbegin(),
            prg->randI(od-1))->second.neighbour();
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
