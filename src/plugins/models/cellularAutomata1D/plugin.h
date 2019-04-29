/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef CELLULARAUTOMATA1D_H
#define CELLULARAUTOMATA1D_H

#include <bitset>
#include <plugininterface.h>

namespace evoplex {
class CellularAutomata1D: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    int m_currRow;

    int m_stateAttrId;  // the id of the `state` node attribute
    std::bitset<8> m_binrule;  // binary representation of the automaton rule

    bool m_toroidal;    // true if the graph is a toroid
    int m_width;        // the number of columns in the `squareGrid` graph
    int m_height;       // the number of rows in the `squareGrid` graph

    // returns the next state of a node based on the state
    // of itself and its neighbours on the left and right
    Value nextState(const Node& leftNode, const Node& node, const Node& rightNode) const;

    // return the linear index of an element in a matrix.
    int linearIdx(int row, int col) const;
};
} // evoplex
#endif // CELLULARAUTOMATA1D_H
