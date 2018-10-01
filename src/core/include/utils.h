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

#ifndef UTILS_H
#define UTILS_H

#include <QHash>
#include <map>
#include <math.h>
#include <unordered_set>
#include <vector>

#include "prg.h"

namespace evoplex
{
namespace Utils
{
    template <class T>
    void deleteAndShrink(std::vector<T*>& v) {
        qDeleteAll(v);
        v.clear();
        std::vector<T*> ve;
        ve.swap(v);
    }

    template <class T>
    void clearAndShrink(std::vector<T>& v) {
        v.clear();
        v.shrink_to_fit();
    }

    template <typename T, class C>
    void deleteAndShrink(QHash<T, C*>& h) {
        qDeleteAll(h);
        h.clear();
        h.squeeze();
    }

    template <typename T, class C>
    void deleteAndShrink(std::map<T, C*>& m) {
        for (auto& i : m) {
            delete i.second;
            i.second = nullptr;
        }
        m.clear();
    }

    template <class C>
    void deleteAndShrink(std::unordered_set<C*>& s) {
        for (C* c : s) {
            delete c;
            c = nullptr;
        }
        s.clear();
    }

    /**
     * @brief Find index of value in range.
     * Returns the index of the first element in the range that compares equal to val
     * If no such element is found, the function returns -1
     */
    template <class C, class T>
    int indexOf(const C& container, const T& val) {
        Q_ASSERT(container.size() < INT32_MAX);
        const int s = static_cast<int>(container.size());
        for (int idx = 0; idx < s; ++idx) {
            if (container.at(idx) == val) return idx;
        }
        return -1;
    }

    /**
     * @brief Fisher–Yates shuffle algorithm.
     * https://bost.ocks.org/mike/shuffle
     * http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
     */
    template <typename T>
    void shuffle(std::vector<T>& vector, PRG* prg) {
        size_t n = vector.size();
        // While there remain elements to shuffle…
        while (n) {
            // Pick a remaining element…
            const size_t i = std::floor(prg->uniform() * n--);
            // And swap it with the current element.
            const T t = vector[n];
            vector[n] = vector[i];
            vector[i] = t;
        }
    }

} // utils
} // evoplex
#endif // UTILS_H
