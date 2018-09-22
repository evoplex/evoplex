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
