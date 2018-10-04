/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EDGES_H
#define EDGES_H

#include <unordered_map>

#include "edge.h"

namespace evoplex {

/**
 * @brief An Edge container.
 * It is an unordered_map with the edge's id as the key.
 * @see Edge
 * @ingroup PublicAPI
 */
class Edges : private std::unordered_map<int, Edge>
{
    friend class AbstractGraph;
    friend class DNode;
    friend class UNode;

public:
    using std::unordered_map<int, Edge>::at;
    using std::unordered_map<int, Edge>::begin;
    using std::unordered_map<int, Edge>::cbegin;
    using std::unordered_map<int, Edge>::end;
    using std::unordered_map<int, Edge>::cend;
    using std::unordered_map<int, Edge>::iterator;
    using std::unordered_map<int, Edge>::const_iterator;
    using std::unordered_map<int, Edge>::empty;
    using std::unordered_map<int, Edge>::size;
};

} // evoplex
#endif // EDGES_H
