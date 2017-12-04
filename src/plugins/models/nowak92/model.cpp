/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "model.h"

namespace evoplex {

bool ModelNowak::init()
{
    m_temptation = attr("temptation").toDouble;
    return m_temptation >=1.0 && m_temptation <= 2.0;
}

bool ModelNowak::algorithmStep()
{
    for (Agent* agent : graph()->agents()) {
        const int sX = agent->attr(Strategy).toInt;
        double score = 0.0;
        for (const Edge* edge : agent->edges()) {
            score += playGame(sX, edge->neighbour()->attr(Strategy).toInt);
        }
        agent->setAttr(Score, score);
    }

    for (Agent* agent : graph()->agents()) {
        int bestStrategy = agent->attr(Strategy).toInt;
        double highestScore = agent->attr(Score).toDouble;
        for (const Edge* edge : agent->edges()) {
            double neighbourScore = edge->neighbour()->attr(Score).toDouble;
            if (!qFuzzyCompare(neighbourScore, highestScore) && neighbourScore > highestScore) {
                highestScore = neighbourScore;
                bestStrategy = edge->neighbour()->attr(Strategy).toInt;
            }
        }
        agent->setAttr(Strategy, bestStrategy);
    }

    return false;
}

double ModelNowak::playGame(int sX, int sY)
{
    switch (sX * 2 + sY) {
    case 0: return 1.0;             // CC : Reward for mutual cooperation
    case 1: return 0.0;             // CD : Sucker's payoff
    case 2: return m_temptation;    // DC : Temptation to defect
    case 3: return 0.0;             // DD : Punishment for mutual defection
    default:
        qFatal("[ModelNowak::playGame()] Error! strategy should be 0 or 1!");
    }
}

} // evoplex

REGISTER_MODEL(ModelNowak)
#include "model.moc"
