/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTIL_H
#define UTIL_H

#include <QVector>

namespace Util {

    // Fisher–Yates shuffle algorithm
    // https://bost.ocks.org/mike/shuffle
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    QVector<int> shuffle(QVector<int> list);

    // convert a linear index to row column
    void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return a linear index
    int getLinearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }
}

#endif // UTIL_H
