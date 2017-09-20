#include <QtDebug>

#include "model.h"

ModelHOPD::ModelHOPD() {}

bool ModelHOPD::init()
{
    return true;
}

bool ModelHOPD::algorithmStep()
{
    // In one MCS, each player is selected once on average
    const int popSize = graph()->getAgents().size();
    for (int innerStep = 0; innerStep < popSize; ++innerStep) {
        // 1. Pick an agent at random (x)
        AbstractAgent* agentX = graph()->getRandomAgent();
        const int strategyX = agentX->attribute(STRATEGY).toInt;
        const Edges* edgesX = agentX->getEdges();

        // 2. play the game with each neighbour and calculate utility of x
        double uX = 0.0; // total utility of x
        for (int nId = 0; nId < edgesX->size(); ++nId) {
            const AbstractAgent* neighbour = edgesX->at(nId).getNeighbour();
            uX += playGame(strategyX, neighbour->attribute(STRATEGY).toInt);
        }

        // 3. select a neighbour of x at random (y)
        const int idY = prg()->randI(edgesX->size());
        const AbstractAgent* agentY = edgesX->at(idY).getNeighbour();
        const int strategyY = agentY->attribute(STRATEGY).toInt;
        const Edges* edgesY = agentY->getEdges();

        // 4. calculate utility of y
        double uY = 0.0; // total utility of y
        for (int nId = 0; nId < edgesY->size(); ++nId) {
            const AbstractAgent* neighbour = edgesY->at(nId).getNeighbour();
            uY += playGame(strategyY, neighbour->attribute(STRATEGY).toInt);
        }

        // 5. update strategy
        double prob = 1. / (1. + exp((-(uY - uX) / 8.0) / 0.1));
        if (prob  >  prg()->randD()) {
            agentX->setAttribute(STRATEGY, strategyY);
            agentX->setAttribute(PROBTOA, agentY->attribute(PROBTOA));
        }
    }

    return false;
}

double ModelHOPD::playGame(int sA, int sB) const
{
    if (sA == 2 || sB == 2) {
        return 2.;
        //return m_l;
    }

    switch (sA*2 + sB) {
        case 0:
            return 3.0; // CC : Reward for mutual cooperation
            return 1.0; // CC : Reward for mutual cooperation
        case 1:
            return 0.0; // CD : Sucker's payoff
            return 0.0; // CD : Sucker's payoff
        case 2:
            return 5.0; // DC : Temptation to defect
            //return m_b; // DC : Temptation to defect
        case 3:
            return 1.0; // DD : Punishment for mutual defection
            return 0.0; // DD : Punishment for mutual defection
        default: // it should never happen
            qDebug() << sA << sB;
            qFatal("[HOPD::playGame()] Error!");
    }
}
