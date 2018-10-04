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

#ifndef NODES_P_H
#define NODES_P_H

#include <functional>
#include <unordered_map>

#include "attributerange.h"
#include "enum.h"
#include "nodes.h"

namespace evoplex {

/**
 * @brief A collection of utility functions for creating and saving nodes.
 */
class NodesPrivate
{
public:
    // Expected commands:
    //     - same mode for all attributes:
    //         '*integer;[min|max|rand_seed]'
    //     - specific mode for each attribute:
    //         '#integer;attrName_[min|max|rand_seed|value_val];...'
    static Nodes fromCmd(const QString& cmd, const AttributesScope& attrsScope,
                         const GraphType& graphType, QString& error,
                         std::function<void(int)> progress = [](int){});

    // Read a set of nodes from a csv file
    // Return empty if something goes wrong
    static Nodes fromFile(const QString& filePath, const AttributesScope& attrsScope,
                          const GraphType& graphType, QString& error,
                          std::function<void(int)> progress = [](int){});

    // Export set of nodes to a csv file
    // Return true if successful
    static bool saveToFile(const Nodes& nodes, QString filepath,
                           std::function<void(int)> progress = [](int){});

    // clone a Nodes container
    static Nodes clone(const Nodes& nodes);

private:
    // Checks if the header is in comma-separated format,
    // don't have duplicates, has (or not) 2d coordinates ('x' and 'y')
    // and has all the required attributes (attrsScope)
    static QStringList validateHeader(const QString& header,
            const AttributesScope& attrsScope, QString& error);

    static Node readRow(const int row, const QStringList& header,
            const QStringList& values, const AttributesScope& attrsScope,
            const bool isDirected, QString& error);
};

} // evoplex
#endif // NODES_P_H
