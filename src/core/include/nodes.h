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

#include <functional>
#include <memory>
#include <unordered_map>

#include "attributerange.h"
#include "enum.h"
#include "node.h"

namespace evoplex {

class Nodes : public std::unordered_map<int, NodePtr>
{
public:
    struct Iterator : public iterator {
        Iterator() {}
        Iterator(const iterator& _a) : iterator(_a) {}
        const int& id() const { return (*this)->first; }
        NodePtr& node() const { return (*this)->second; }
    };

    struct ConstIterator : public const_iterator {
        ConstIterator() {}
        ConstIterator(const const_iterator& _a) : const_iterator(_a) {}
        const int& id() const { return (*this)->first; }
        const NodePtr& node() const { return (*this)->second; }
    };

    struct Pair {
        const std::pair<const int, NodePtr>& _p;
        Pair(const std::pair<const int, NodePtr>& _a) : _p(_a) {}
        const int& id() const { return _p.first; }
        const NodePtr& node() const { return _p.second; }
    };

    // Expected commands:
    //     - same mode for all attributes:
    //         '*integer;[min|max|rand_seed]'
    //     - specific mode for each attribute:
    //         '#integer;attrName_[min|max|rand_seed|value_val];...'
    static Nodes fromCmd(const QString& cmd, const AttributesScope& attrsScope,
            const GraphType& graphType, QString& error, std::function<void(int)> progress = [](int){});

    // Read a set of nodes from a csv file
    // Return empty if something goes wrong
    static Nodes fromFile(const QString& filePath, const AttributesScope& attrsScope,
            const GraphType& graphType, QString& error, std::function<void(int)> progress = [](int){});

    // Export set of nodes to a csv file
    // Return true if successful
    bool saveToFile(QString filepath, std::function<void(int)> progress = [](int){}) const;

private:
    // Checks if the header is in comma-separated format,
    // don't have duplicates, has (or not) 2d coordinates ('x' and 'y')
    // and has all the required attributes (attrsScope)
    static QStringList validateHeader(const QString& header,
            const AttributesScope& attrsScope, QString& error);

    static NodePtr readRow(const int row, const QStringList& header,
            const QStringList& values, const AttributesScope& attrsScope,
            const bool isDirected, QString& error);
};

} // evoplex
#endif // NODES_H
