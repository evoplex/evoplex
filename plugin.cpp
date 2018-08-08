/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool PopulationGrowth::init()
{
    m_infectedAttrId = AbstractModel::nodes().at(0)->attrs().indexOf("infected");

    // initializing model attribute, which is constant throughout the simulation
    m_prob = attr("prob").toDouble();

    return m_infectedAttrId >= 0;
}

bool PopulationGrowth::algorithmStep()
{
    std::vector<bool> nextInfectedStates;
    nextInfectedStates.reserve(nodes().size());

    // For each node
    for (Nodes::Pair np : nodes()) {
        NodePtr currentNode = np.node();

        // Check if the current node is currently healthy
        // i.e. if the node is already infected, there's nothing to do
        if (currentNode->attr(m_infectedAttrId).toBool()) {
            nextInfectedStates.emplace_back(true);
            continue;
        }

        // Select a random neighbour
        const NodePtr neighbour = currentNode->randNeighbour(AbstractModel::prg());

        // Check if the neighbour is currently infected
        if (neighbour->attrs().value(m_infectedAttrId).toBool()) {
            const bool infected = m_prob > prg()->randD();
            nextInfectedStates.emplace_back(infected);
        } else {
            nextInfectedStates.emplace_back(false);
        }
    }

    // For each node, load the next state into the current state
    size_t i = 0;
    for (Nodes::Pair np : nodes()) {
        NodePtr currentNode = np.node();

        // if the node is already infected, there's nothing to do
        if (currentNode->attr(m_infectedAttrId).toBool()) {
            ++i;
            continue;
        }

        bool inf = Value(nextInfectedStates.at(i)).toBool();
        currentNode->setAttr(m_infectedAttrId, inf);
        ++i;
    }

    return true;
}
} // evoplex
REGISTER_PLUGIN(PopulationGrowth)
#include "plugin.moc"
