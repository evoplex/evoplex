/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef STATS_H
#define STATS_H

#include "agent.h"
#include "attributes.h"

namespace evoplex {

class Stats
{
public:

    enum Function {
        Count
    };

    enum Entity {
        Agents,
        Edges
    };

    // <Function, Entity, attrIdx>
    typedef std::tuple<Function, Entity, int> Operation;

    static void doOperation(Operation op)
    {
        // TODO
    }

    static void count(const Agents& agents, const int attrIdx,
                      std::vector<Value>& values, std::vector<int>& count)
    {
        for (const Agent* agent : agents) {
            int i = std::find(values.begin(), values.end(), agent->attr(attrIdx)) - values.begin();
            if (i == values.size()) { // not found
                values.emplace_back(agent->attr(attrIdx));
                count.emplace_back(1);
            } else {
                ++count[i];
            }
        }
    }
};
}
#endif // UTILS_H
