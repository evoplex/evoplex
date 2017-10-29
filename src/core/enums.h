/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ENUMS_H
#define ENUMS_H

namespace evoplex {

enum GraphType {
    Invalid_Type,
    Directed,
    Undirected
};

static GraphType enumFromString(const QString& str)
{
    if (str == "directed") return Directed;
    if (str == "undirected") return Undirected;
    return Invalid_Type;
}

}

#endif // ENUMS_H
