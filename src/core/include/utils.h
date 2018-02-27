/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
    static void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return the linear index of an element in a matrix.
    static int linearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    static int linearIdx(std::pair<int,int> rowCol, const int cols) {
        return linearIdx(rowCol.first, rowCol.second, cols);
    }

} // utils
} // evoplex
#endif // UTILS_H
