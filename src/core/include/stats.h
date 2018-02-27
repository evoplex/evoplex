/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef STATS_H
#define STATS_H

#include <vector>

#include "attributes.h"

namespace evoplex {

class Stats
{
public:
    template<typename Iterator>
    static std::vector<Value> count(Iterator entityBegin, Iterator entityEnd,
                                    const int attrIdx, std::vector<Value> header)
    {
        std::vector<Value> ret(header.size(), 0);
        while (entityBegin != entityEnd) {
            int i = std::find(header.begin(), header.end(), (*entityBegin)->attr(attrIdx)) - header.begin();
            if (i != header.size()) {
                ret[i] = ret[i].toInt() + 1;
            }
            ++entityBegin;
        }
        return ret;
    }

    template<typename Container>
    static std::vector<Value> count(Container entity, const int attrIdx, std::vector<Value> values)
    {
        return count(entity.begin(), entity.end(), attrIdx, values);
    }
};

}
#endif // STATS_H
