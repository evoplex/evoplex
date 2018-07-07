/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef EDGES_H
#define EDGES_H

#include <memory>
#include <unordered_map>

#include "edge.h"

namespace evoplex {

struct Edges : public std::unordered_map<int, EdgePtr>
{
public:
    struct Iterator : public iterator {
        Iterator() {}
        Iterator(const iterator& _a) : iterator(_a) {}
        const int& id() const { return (*this)->first; }
        EdgePtr& edge() const { return (*this)->second; }
    };

    struct ConstIterator : public const_iterator {
        ConstIterator() {}
        ConstIterator(const const_iterator& _a) : const_iterator(_a) {}
        const int& id() const { return (*this)->first; }
        const EdgePtr& edge() const { return (*this)->second; }
    };

    struct Pair {
        const std::pair<const int, EdgePtr>& _p;
        Pair(const std::pair<const int, EdgePtr>& _a) : _p(_a) {}
        const int& id() const { return _p.first; }
        const EdgePtr& edge() const { return _p.second; }
    };
};

} // evoplex
#endif // EDGES_H
