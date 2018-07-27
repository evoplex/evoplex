/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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
    void deleteAndShrink(std::vector<T>& v) {
        v.clear();
        std::vector<T> ve;
        ve.swap(v);
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

    // Find index of value in range.
    // Returns the index of the first element in the range that compares equal to val
    // If no such element is found, the function returns -1
    template <class C, class T>
    int indexOf(const C& container, const T& val) {
        Q_ASSERT(container.size() < INT32_MAX);
        const int s = static_cast<int>(container.size());
        for (int idx = 0; idx < s; ++idx) {
            if (container.at(idx) == val) return idx;
        }
        return -1;
    }

    // clone a population of nodes
    template <class C>
    C clone(const C& container) {
        C ret;
        ret.reserve(container.size());
        for (auto const& pair : container) {
            ret.insert({pair.first, pair.second->clone()});
        }
        return ret;
    }

    // Fisher–Yates shuffle algorithm
    // https://bost.ocks.org/mike/shuffle
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    template <typename T>
    void shuffle(std::vector<T>& vector, PRG* prg) {
        size_t n = vector.size();
        // While there remain elements to shuffle…
        while (n) {
            // Pick a remaining element…
            const size_t i = std::floor(prg->randD() * n--);
            // And swap it with the current element.
            const T t = vector[n];
            vector[n] = vector[i];
            vector[i] = t;
        }
    }

} // utils
} // evoplex
#endif // UTILS_H
