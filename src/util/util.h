/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTIL_H
#define UTIL_H

namespace Util {

    //! Fisher–Yates shuffle algorithm
    //! https://bost.ocks.org/mike/shuffle
    //! http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    QVector<int> shuffle(QVector<int> list);

}

#endif // UTIL_H
