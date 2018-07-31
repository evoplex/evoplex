/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool MinimalModel::init()
{
    m_live = AbstractModel::nodes().at(0)->attrs().indexOf("live");
    m_live_next_state = AbstractModel::nodes().at(0)->attrs().indexOf("live_next_state");

    return true;
}

bool MinimalModel::algorithmStep()
{
    int liveNeighbourCount;
    NodePtr currentNode, neighbour;
    bool nextState;

    for (Nodes::Pair np : nodes()) {
        currentNode = np.node();
        liveNeighbourCount = 0;

        for (Edges::Pair ep : np.node()->outEdges()) {
            neighbour = ep.edge()->neighbour();

            // If the neighbour is alive
            if (neighbour->attrs().value(m_live).toBool()) {
                liveNeighbourCount++;
            }
        }

        if (currentNode->attrs().value(m_live).toBool()) {
            if (liveNeighbourCount < 2) { // Case 1: Underpopulation
                currentNode->setAttr(m_live_next_state, false);
            } else if ((liveNeighbourCount == 2)||(liveNeighbourCount == 3)) { // Case 2: Lives to next state
                currentNode->setAttr(m_live_next_state, true);
            } else if (liveNeighbourCount > 3) { // Case 3: Overpopulation
                currentNode->setAttr(m_live_next_state, false);
            }
        } else {
           if (liveNeighbourCount == 3) { // Case 4: Reproduction
               currentNode->setAttr(m_live_next_state, true);
           }
       }
    }

    for (Nodes::Pair np : nodes()) {
        currentNode = np.node();
        nextState = currentNode->attrs().value(m_live_next_state).toBool();
        currentNode->setAttr(m_live, nextState);
    }
    return true;
}

} // evoplex
REGISTER_PLUGIN(MinimalModel)
#include "plugin.moc"
