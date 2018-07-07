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
#include <vector>
#include <unordered_set>

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

    // convert a linear index to row and column
    static inline void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return the linear index of an element in a matrix.
    static inline int linearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    static inline int linearIdx(std::pair<int,int> rowCol, const int cols) {
        return linearIdx(rowCol.first, rowCol.second, cols);
    }

} // utils
} // evoplex
#endif // UTILS_H
