/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "model.h"

namespace evoplex {

bool ModelNowak::init()
{
    m_temptation = attr("temptation").toDouble();
    return m_temptation >=1.0 && m_temptation <= 2.0;
}

bool ModelNowak::algorithmStep()
{
    std::vector<char> bestStrategies;
    bestStrategies.reserve(graph()->agents().size());

    for (Agent* agent : graph()->agents()) {
        int bestStrategy = agent->attr(Strategy).toInt();
        double highestScore = agent->attr(Score).toDouble();
        for (const Edge* edge : agent->edges()) {
            const double neighbourScore = edge->neighbour()->attr(Score).toDouble();
            if (neighbourScore > highestScore) {
                highestScore = neighbourScore;
                bestStrategy = edge->neighbour()->attr(Strategy).toInt();
            }
        }
        bestStrategies.emplace_back(binarize(bestStrategy));
    }

    int i = 0;
    for (Agent* agent : graph()->agents()) {
        int s = binarize(agent->attr(Strategy).toInt());
        s = (s == bestStrategies.at(i)) ? s : bestStrategies.at(i) + 2;
        agent->setAttr(Strategy, s);
        ++i;
    }

    for (Agent* agent : graph()->agents()) {
        const int sX = agent->attr(Strategy).toInt();
        double score = 0.0;
        for (const Edge* edge : agent->edges()) {
            score += playGame(sX, edge->neighbour()->attr(Strategy).toInt());
        }
        agent->setAttr(Score, score);
    }

    return false;
}

// 0) cooperator; 1) new cooperator
// 2) defector;   3) new defector
const double ModelNowak::playGame(const int sX, const int sY) const
{
    switch (binarize(sX) * 2 + binarize(sY)) {
    case 0: return 1.0;             // CC : Reward for mutual cooperation
    case 1: return 0.0;             // CD : Sucker's payoff
    case 2: return m_temptation;    // DC : Temptation to defect
    case 3: return 0.0;             // DD : Punishment for mutual defection
    default:
        qFatal("[ModelNowak::playGame()] Error! strategy should be 0 or 1!");
    }
}

// transform from 0|2 -> 0 (cooperator)
//                1|3 -> 1 (defector)
const int ModelNowak::binarize(const int strategy) const
{
    return (strategy < 2) ? strategy : strategy - 2;
}

} // evoplex

REGISTER_MODEL(ModelNowak)
#include "model.moc"
