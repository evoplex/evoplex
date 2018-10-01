/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STATS_H
#define STATS_H

#include <vector>

#include "attributes.h"

namespace evoplex {

class Stats
{
public:
    /**
     * @brief Count frequency of the header values in the container.
     *
     * Example:
     *  - if header is ['a', 'c'], it'll count how many a's and c's exist in the container
     *  - if there are 100 a's and 123 c's, it'll return [100, 123]
     *  - if there are no a's and c's, it'll return [0, 0]
     */
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
