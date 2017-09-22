#include <QtDebug>

#include "model.h"

ModelHOPD::ModelHOPD() {}

bool ModelHOPD::init()
{
    m_mutRate = attributes()->value(MutRate);

    return true;
}

bool ModelHOPD::algorithmStep()
{
    // In one MCS, each player is selected once on average
    const int popSize = graph()->getAgents().size();
    for (int innerStep = 0; innerStep < popSize; ++innerStep) {
        // 1. Pick an agent at random (x)
        AbstractAgent* agentX = graph()->getRandomAgent();
        const Edges* edgesX = agentX->getEdges();

        // 2. mutation of the attribute 'ProbToA'
        if (m_mutRate > 0.0 && m_mutRate > prg()->randD()) {
            agentX->setAttribute(PROBTOA, prg()->randD());
        }

        // 2. play the game with each neighbour and calculate utility of x
        double uX = 0.0; // total utility of x
        for (int nId = 0; nId < edgesX->size(); ++nId) {
            uX += playGame(agentX, edgesX->at(nId).getNeighbour());
        }

        // 3. select a neighbour of x at random (y)
        const int idY = prg()->randI(edgesX->size());
        AbstractAgent* agentY = edgesX->at(idY).getNeighbour();
        const Edges* edgesY = agentY->getEdges();

        // 4. calculate utility of y
        double uY = 0.0; // total utility of y
        for (int nId = 0; nId < edgesY->size(); ++nId) {
            uY += playGame(agentY, edgesY->at(nId).getNeighbour());
        }

        // 5. update strategy
        double prob = 1. / (1. + exp((-(uY - uX) / 8.0) / 0.1));
        if (prob  >  prg()->randD()) {
            agentX->setAttribute(STRATEGY, agentY->attribute(STRATEGY));
            agentX->setAttribute(PROBTOA, agentY->attribute(PROBTOA));
        }
    }

    return false;
}

double ModelHOPD::playGame(AbstractAgent* agentX, AbstractAgent* agentY) const
{
    // check if one of the agents do not want to play
    const double p = prg()->randD();
    if (agentX->attribute(PROBTOA).toDouble > p
            || agentY->attribute(PROBTOA).toDouble > p)
        return 2.0;

    const int sX = agentX->attribute(STRATEGY).toInt;
    const int sY = agentY->attribute(STRATEGY).toInt;
    switch (sX*2 + sY) {
        case 0: // CC : Reward for mutual cooperation
            return 3.0;
            return 1.0;
        case 1: // CD : Sucker's payoff
            return 0.0;
        case 2: // DC : Temptation to defect
            return 5.0;
            //return m_b;
        case 3: // DD : Punishment for mutual defection
            return 1.0;
            return 0.0;
        default: // it should never happen
            qDebug() << sX << sY;
            qFatal("[HOPD::playGame()] Error!");
    }
}
