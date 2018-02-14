/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <QFileInfo>
#include <QVariant>
#include <QVector>
#include <QtDebug>
#include <float.h>

#include "agent.h"
#include "attributes.h"
#include "prg.h"
#include "valuespace.h"

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
        }
        m.clear();
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
