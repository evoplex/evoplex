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

namespace evoplex {

class Utils
{
public:

    // convert a linear index to row and column
    static void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return the linear index of an element in a matrix.
    static int linearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    static int linearIdx(QPair<int,int> rowCol, const int cols) {
        return linearIdx(rowCol.first, rowCol.second, cols);
    }
};
}
#endif // UTILS_H
