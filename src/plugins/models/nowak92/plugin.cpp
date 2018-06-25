/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "plugin.h"

namespace evoplex {

bool ModelNowak::init()
{
    m_temptation = attr("temptation", -1.0).toDouble();
    return m_temptation >=1.0 && m_temptation <= 2.0;
}

bool ModelNowak::algorithmStep()
{
    // 1. each agent accumulates the payoff obtained by playing the game with all its neighbours and itself
    for (Node* node : graph()->nodes()) {
        const int sX = node->attr(Strategy).toInt();
        double score = playGame(sX, sX);
        for (const Edge* edge : node->edges()) {
            score += playGame(sX, edge->neighbour()->attr(Strategy).toInt());
        }
        node->setAttr(Score, score);
    }

    std::vector<char> bestStrategies;
    bestStrategies.reserve(graph()->nodes().size());

    // 2. the best agent in the neighbourhood is selected to reproduce
    for (Node* node : graph()->nodes()) {
        int bestStrategy = node->attr(Strategy).toInt();
        double highestScore = node->attr(Score).toDouble();
        for (const Edge* edge : node->edges()) {
            const double neighbourScore = edge->neighbour()->attr(Score).toDouble();
            if (neighbourScore > highestScore) {
                highestScore = neighbourScore;
                bestStrategy = edge->neighbour()->attr(Strategy).toInt();
            }
        }
        bestStrategies.emplace_back(binarize(bestStrategy));
    }

    // 3. prepare the next generation
    int i = 0;
    for (Node* node : graph()->nodes()) {
        int s = binarize(node->attr(Strategy).toInt());
        s = (s == bestStrategies.at(i)) ? s : bestStrategies.at(i) + 2;
        node->setAttr(Strategy, s);
        ++i;
    }

    return false;
}

// 0) cooperator; 1) new cooperator
// 2) defector;   3) new defector
double ModelNowak::playGame(const int sX, const int sY) const
{
    switch (binarize(sX) * 2 + binarize(sY)) {
    case 0: return 1.0;             // CC : Reward for mutual cooperation
    case 1: return 0.0;             // CD : Sucker's payoff
    case 2: return m_temptation;    // DC : Temptation to defect
    case 3: return 0.0;             // DD : Punishment for mutual defection
    default:
        qFatal("Error! strategy should be 0 or 1!");
        return 0.0; // supress compiler warning
    }
}

// transform from 0|2 -> 0 (cooperator)
//                1|3 -> 1 (defector)
int ModelNowak::binarize(const int strategy) const
{
    return (strategy < 2) ? strategy : strategy - 2;
}

} // evoplex

REGISTER_MODEL(ModelNowak)
#include "plugin.moc"
