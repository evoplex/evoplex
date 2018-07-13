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

#ifndef ENUM_H
#define ENUM_H

#include <QString>

namespace evoplex {

/**
 * We need an efficient and simple way to convert enums to strings (vice-versa),
 * many times we also need to catch invalid strings as Invalid. Also, sometimes
 * the string does not reflect the enum name.
 * There are many ways to do that, we have tried Q_ENUM, 'better-enums' and
 * other solutions involving more complex macros, but none worked well here.
 * The decision so far was to use a simple function to handle that. It
 * is a naive solution but it's easier to use and maintain (as long as we
 * don't have big enums).
 */
template <typename T>
T _enumFromString(const QString& str);

enum class Status {
    Invalid,  // something went wrong
    Unset,    // has not yet been initialized
    Ready,    // ready for another step
    Queued,   // queued to run
    Running,  // running in a work thread
    Finished, // all is done
};

enum class PluginType : int {
    Invalid = 0,
    Graph = 1,
    Model = 2
};
template<>
inline PluginType _enumFromString<PluginType>(const QString& str) {
    if (str == "graph") return PluginType::Graph;
    if (str == "model") return PluginType::Model;
    return PluginType::Invalid;
}

enum class GraphType : int {
    Invalid = 0,
    Undirected = 1,
    Directed = 2
};
template<>
inline GraphType _enumFromString<GraphType>(const QString& str) {
    if (str == "undirected") return GraphType::Undirected;
    if (str == "directed") return GraphType::Directed;
    return GraphType::Invalid;
}

} // evoplex
#endif // ENUM_H
