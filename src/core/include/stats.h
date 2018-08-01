/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef STATS_H
#define STATS_H

#include <vector>

#include "attributes.h"

namespace evoplex {

class Stats
{
public:
    // count frequency of the header values in the container
    // eg.,
    //     if header is ['a', 'c'], it'll count how many a's and c's exist in the container
    //     if there are 100 a's and 123 c's, it'll return [100, 123]
    //     if there are no a's and c's, it'll return [0, 0]
    template<typename ConstIterator>
    static std::vector<Value> count(ConstIterator entityBegin, ConstIterator entityEnd,
                                    const int attrIdx, std::vector<Value> header)
    {
        std::vector<Value> ret(header.size(), 0);
        while (entityBegin != entityEnd) {
            const size_t i = std::find(header.begin(), header.end(), entityBegin->second.attr(attrIdx)) - header.begin();
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
        return count(entity.cbegin(), entity.cend(), attrIdx, values);
    }
};

}
#endif // STATS_H
