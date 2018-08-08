/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool GameOfLifeModel::init()
{
    m_live = node(0).attrs().indexOf("live");

    return true;
}

bool GameOfLifeModel::algorithmStep()
{
    std::vector<bool> nextInfectedStates;
    nextInfectedStates.reserve(nodes().size());

    for (Node node : nodes()) {
        int liveNeighbourCount = 0;

        for(Edge edge : node.outEdges()){
            Node neighbour = edge.neighbour();

            // If the neighbour is alive
            if (neighbour.attrs().value(m_live).toBool()) {
                liveNeighbourCount++;
            }
        }

        if (node.attrs().value(m_live).toBool()) { // If the cell is live
            if (liveNeighbourCount < 2) { // Underpopulation
                nextInfectedStates.emplace_back(false);
            } else if ((liveNeighbourCount == 2)||(liveNeighbourCount == 3)) { // Lives to next state
                nextInfectedStates.emplace_back(true);
            }  else if (liveNeighbourCount > 3){ // Overpopulation
                nextInfectedStates.emplace_back(false);
            }
        } else {
            if (liveNeighbourCount == 3) { // Reproduction
               nextInfectedStates.emplace_back(true);
            } else { // Remains dead
                nextInfectedStates.emplace_back(false);
            }
        }
    }

    // For each node, load the next state into the current state
    size_t i = 0;
    for (Node node : nodes()) {
        node.setAttr(m_live, Value(nextInfectedStates.at(0)).toBool());
        ++i;
    }
    return true;
}

} // evoplex
REGISTER_PLUGIN(GameOfLifeModel)
#include "plugin.moc"
