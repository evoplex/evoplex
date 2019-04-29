/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include "plugin.h"

namespace evoplex {

bool CellularAutomata1D::init()
{
    // this model is only valid for `squareGrid` graph
    if (graphId() != "squareGrid" || !graph()->attrExists("boundary") ||
            !graph()->attrExists("width") || !graph()->attrExists("height")) {
        return false;
    }

    // starts reading from row=0
    m_currRow = 0;

    // checks if the `squareGrid` was set with periodic bondary conditions (i.e., a toroid)
    m_toroidal = graph()->attr("boundary") == "periodic";
    // gets the `squareGrid` width
    m_width = graph()->attr("width").toInt();
    // gets the `squareGrid` height
    m_height = graph()->attr("height").toInt();

    if (m_height < 2) {
        qWarning() << "the 'squareGrid' height must be >= 2";
        return false;
    }

    // gets the id of the `state` node's attribute, which is the same for all nodes
    m_stateAttrId = node(0).attrs().indexOf("state");

    // determines which rule to use
    m_binrule = std::bitset<8>(attr("rule").toInt());

    return m_stateAttrId >= 0;
}

bool CellularAutomata1D::algorithmStep()
{
    // 1. gets first node in the current row
    Node first = node(linearIdx(m_currRow, 0));

    // 2. for each node (starting from the second),
    int lastColumn = m_width - 1;
    for (int col = 1; col < lastColumn; ++col) {
        Node central = node(first.id() + col);
        // a. compute the next state based on its neighbours on the left and right
        Value state = nextState(node(central.id()-1), central, node(central.id()+1));
        // b. assign the next state to the node below the current node
        node(central.id()+m_width).setAttr(m_stateAttrId, state);
    }

    // 3. edge case: if the graph is a toroid, we compute the state for the last column
    if (m_toroidal) {
        Node last = node(linearIdx(m_currRow, lastColumn));
        // a. compute the next state based on its left neighbour and the first node in the row
        Value state = nextState(node(last.id()-1), last, first);
        // b. assign the next state to the first node of the next row
        node(first.id()+m_width).setAttr(m_stateAttrId, state);
    }

    ++m_currRow;
    if (m_currRow == m_height-1) {
        // all rows have been filled; return false to stop the simulation
        return false;
    }
    return true;
}

Value CellularAutomata1D::nextState(const Node& leftNode, const Node& node, const Node& rightNode) const
{
    bool left = leftNode.attr(m_stateAttrId).toBool();
    bool center = node.attr(m_stateAttrId).toBool();
    bool right = rightNode.attr(m_stateAttrId).toBool();

    return Value(m_binrule[left*4 + center*2 + right]);
}

int CellularAutomata1D::linearIdx(int row, int col) const
{
    return row * m_width + col;
}

} // evoplex
REGISTER_PLUGIN(CellularAutomata1D)
#include "plugin.moc"
