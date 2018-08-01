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

#ifndef NODES_H
#define NODES_H

#include <unordered_map>

#include "node.h"

namespace evoplex {

class Nodes : private std::unordered_map<int, Node>
{
    friend class AbstractGraph;
    friend class Experiment;
    friend class NodesPrivate;

public:
    using std::unordered_map<int, Node>::at;
    using std::unordered_map<int, Node>::begin;
    using std::unordered_map<int, Node>::cbegin;
    using std::unordered_map<int, Node>::end;
    using std::unordered_map<int, Node>::cend;
    using std::unordered_map<int, Node>::iterator;
    using std::unordered_map<int, Node>::const_iterator;
    using std::unordered_map<int, Node>::empty;
    using std::unordered_map<int, Node>::size;

};

} // evoplex
#endif // NODES_H
