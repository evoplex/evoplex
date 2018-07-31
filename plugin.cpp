/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#include "plugin.h"

namespace evoplex {

bool MinimalModel::init()
{
    m_infected = AbstractModel::nodes().at(0)->attrs().indexOf("infected");
    m_infected_next_state = AbstractModel::nodes().at(0)->attrs().indexOf("infected_next_state");

    // Initializing global variables
    m_prob = attr("prob").toDouble();
    m_number_of_nodes = AbstractModel::nodes().size();

    return m_infected >= 0;
}

bool MinimalModel::algorithmStep()
{
    PRG* prg = new PRG(123);
    NodePtr currentNode, neighbour;
    double randTest;
    bool next_state_infection;

    // For each node
    for(int i = 0; i < m_number_of_nodes; i++){
        currentNode = AbstractModel::nodes().at(i);

        // Check if the current node is currently healthy
        if(!currentNode->attrs().value(m_infected).toBool()){
            // Select a random neighbour
            neighbour = currentNode->randNeighbour(prg);

            // Check if the neighbour is currently infected
            if(neighbour->attrs().value(m_infected).toBool()){
                // Generate a random double in the range [0,1]
                randTest = prg->randD(0,1);

                // Test if the number is less than or equal to the probablilty
                // (The probablity of this being true is the same as the probability itself)
                // Infect the next state of the current node if true
                if(randTest <= m_prob){
                    currentNode->setAttr(m_infected_next_state, true);
                }
            }
        }
    }

    // For each node, load the next state into the current state
    for(int i = 0; i < m_number_of_nodes; i++){
        currentNode = AbstractModel::nodes().at(i);
        next_state_infection = currentNode->attrs().value(m_infected_next_state).toBool();
        currentNode->setAttr(m_infected, next_state_infection);
    }
    return true;
}
} // evoplex
REGISTER_PLUGIN(MinimalModel)
#include "plugin.moc"
