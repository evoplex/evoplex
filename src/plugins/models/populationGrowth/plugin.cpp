/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include "plugin.h"

namespace evoplex {

bool PopulationGrowth::init()
{
    // gets the id of the `infected` node's attribute, which is the same for all nodes
    m_infectedAttrId = node(0).attrs().indexOf("infected");
    // initializing model attribute, which is constant throughout the simulation
    m_prob = attr("prob").toDouble();

    return m_infectedAttrId >= 0;
}

bool PopulationGrowth::algorithmStep()
{
    std::vector<Value> nextInfectedStates;
    nextInfectedStates.reserve(nodes().size());

    for (Node node : nodes()) {
        if (node.attr(m_infectedAttrId).toBool()) {
            nextInfectedStates.emplace_back(true);
            continue; // the node is already infected; skip
        }

        if (node.outDegree() < 1) {
            nextInfectedStates.emplace_back(false);
            continue; // the node does not have neighbours; skip
        }

        // Select a random neighbour
        auto neighbour = node.randNeighbour(prg());

        // and check if the neighbour is currently infected
        if (neighbour.attr(m_infectedAttrId).toBool()) {
            // if so, the current node will become infected with a given probability
            nextInfectedStates.emplace_back(m_prob > prg()->uniform());
        } else {
            nextInfectedStates.emplace_back(false);
        }
    }

    // For each node, load the next state into the current state
    size_t i = 0;
    for (Node node : nodes()) {
        node.setAttr(m_infectedAttrId, nextInfectedStates.at(i));
        ++i;
    }

    return true;
}
} // evoplex
REGISTER_PLUGIN(PopulationGrowth)
#include "plugin.moc"
