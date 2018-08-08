/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool MinimalModel::init()
{
    m_live = node(0).attrs().indexOf("live");

    return true;
}

bool MinimalModel::algorithmStep()
{
    int liveNeighbourCount;
    NodePtr currentNode;
    bool nextState;


    std::vector<bool> nextInfectedStates;
    nextInfectedStates.reserve(nodes().size());

    for (Node node : nodes()) {
        liveNeighbourCount = 0;

        for(Edge edge : node.outEdges()){
            Node neighbour = edge.neighbour();

            // If the neighbour is alive
            if (neighbour.attrs().value(m_live).toBool()) {
                liveNeighbourCount++;
            }
        }



        if (node.attrs().value(m_live).toBool()) {
            if (liveNeighbourCount < 2) { // Case 1: Underpopulation
                nextInfectedStates.emplace_back(false);
            } else if ((liveNeighbourCount == 2)||(liveNeighbourCount == 3)) { // Case 2: Lives to next state
                nextInfectedStates.emplace_back(true);
            } else if (liveNeighbourCount > 3) { // Case 3: Overpopulation
                nextInfectedStates.emplace_back(false);
            }
        } else {
           if (liveNeighbourCount == 3) { // Case 4: Reproduction
               nextInfectedStates.emplace_back(true);
           }
       }
    }


    // For each node, load the next state into the current state
    size_t i = 0;
    for (Node node : nodes()) {
        node.setAttr(m_live, Value(nextInfectedStates.at(i)).toBool());
        ++i;
    }
    return true;
}

} // evoplex
REGISTER_PLUGIN(MinimalModel)
#include "plugin.moc"
