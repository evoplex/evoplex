/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool MinimalModel::init()
{
    m_infectedAttrId = AbstractModel::nodes().at(0)->attrs().indexOf("infected");

    // Initializing global variables
    m_prob = attr("prob").toDouble();

    return m_infectedAttrId >= 0;
}

bool MinimalModel::algorithmStep()
{
    std::vector<bool> nextInfectedStates;
    nextInfectedStates.reserve(nodes().size());

    // For each node
    for (Nodes::Pair np : nodes()) {
        NodePtr currentNode = np.node();

        // if the node is already infected, there's nothing to do
        if (currentNode->attr(m_infectedAttrId).toBool()) {
            continue;
        }

        // Select a random neighbour
        const NodePtr neighbour = currentNode->randNeighbour(AbstractModel::prg());

        // Check if the neighbour is currently infected
        if (neighbour->attrs().value(m_infectedAttrId).toBool()) {
            const bool infected = m_prob > prg()->randD();
            nextInfectedStates.emplace_back(infected);
        }
    }

    // For each node
//    for (int i = 0; i < m_number_of_nodes; ++i) {
//        currentNode = AbstractModel::nodes().at(i);

//        // Check if the current node is currently healthy
//        if (!currentNode->attrs().value(m_infected).toBool()) {
//            // Select a random neighbour
//            neighbour = currentNode->randNeighbour(prg);

//            // Check if the neighbour is currently infected
//            if (neighbour->attrs().value(m_infected).toBool()) {
//                // Generate a random double in the range [0,1]
//                randTest = prg->randD(0,1);

//                // Test if the number is less than or equal to the probablilty
//                // (The probablity of this being true is the same as the probability itself)
//                // Infect the next state of the current node if true
//                if (randTest <= m_prob) {
//                    currentNode->setAttr(m_infected_next_state, true);
//                }
//            }
//        }
//    }
//     For each node, load the next state into the current state
    size_t i = 0;
    for (Nodes::Pair np : nodes()) {
        NodePtr currentNode = np.node();
        currentNode->setAttr(m_infectedAttrId, Value(nextInfectedStates.at(i)).toBool());
        ++i;
    }


//    for (Nodes::Pair np : nodes()) {
//        currentNode = np.node();
//        next_state_infection = currentNode->attrs().value(m_infected+1).toBool();
//        currentNode->setAttr(m_infected, next_state_infection);
//    }
    return true;
}
} // evoplex
REGISTER_PLUGIN(MinimalModel)
#include "plugin.moc"
