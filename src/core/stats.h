/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef STATS_H
#define STATS_H

#include <vector>

namespace evoplex {

class Stats
{
public:
    template<typename Iterator>
    std::vector<int> count(Iterator entityBegin, Iterator entityEnd,
                           const int attrIdx, std::vector<Value> header)
    {
        std::vector<int> count;
        count.resize(header.size());
        while (entityBegin != entityEnd) {
            int i = std::find(header.begin(), header.end(), (*entityBegin)->attr(attrIdx)) - header.begin();
            if (i != header.size()) {
                ++count[i];
            }
            ++entityBegin;
        }
        return count;
    }

    template<typename Container>
    std::vector<int> count(Container entity, const int attrIdx, std::vector<Value> values)
    {
        return count(entity.begin(), entity.end(), attrIdx, values);
    }
};

}
#endif // STATS_H
