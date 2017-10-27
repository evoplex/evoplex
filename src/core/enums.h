/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ENUMS_H
#define ENUMS_H

namespace evoplex {

enum GraphType {
    INVALID_TYPE,
    DIRECTED,
    UNDIRECTED
};

static GraphType enumFromString(const QString& str)
{
    if (str == "directed") return DIRECTED;
    if (str == "undirected") return UNDIRECTED;
    return INVALID_TYPE;
}

}

#endif // ENUMS_H
