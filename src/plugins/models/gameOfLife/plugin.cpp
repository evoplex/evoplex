/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include "plugin.h"

namespace evoplex {

bool GameOfLife::init()
{
    // gets the id of the `live` node's attribute, which is the same for all nodes
    m_liveAttrId = node(0).attrs().indexOf("live");
    return m_liveAttrId >= 0;
}

bool GameOfLife::algorithmStep()
{
    std::vector<Value> nextStates;
    nextStates.reserve(nodes().size());

    for (Node node : nodes()) {
        int liveNeighbourCount = 0;
        for (Node neighbour : node.outEdges()){
            if (neighbour.attr(m_liveAttrId).toBool()) {
                ++liveNeighbourCount;
            }
        }

        if (node.attr(m_liveAttrId).toBool()) {
            if (liveNeighbourCount < 2) { // Dies due to underpopulation
                nextStates.emplace_back(false);
            } else if (liveNeighbourCount < 4) { // Lives to next state
                nextStates.emplace_back(true);
            }  else { // Dies due to overpopulation
                nextStates.emplace_back(false);
            }
        } else {
            // Any dead node with exactly three live neighbors
            // becomes a live node, as if by reproduction.
            nextStates.emplace_back(liveNeighbourCount == 3);
        }
    }

    // For each node, load the next state into the current state
    size_t i = 0;
    for (Node node : nodes()) {
        node.setAttr(m_liveAttrId, nextStates.at(i));
        ++i;
    }
    return true;
}

} // evoplex
REGISTER_PLUGIN(GameOfLife)
#include "plugin.moc"
