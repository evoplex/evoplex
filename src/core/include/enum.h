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

#ifndef ENUM_H
#define ENUM_H

#include <QString>

namespace evoplex {

/**
 * @brief Gets a QString from enum.
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

//! converts an enum to QString
template <typename T>
QString _enumToString(T e);

enum class Status {
    Invalid,   //! something went wrong
    Disabled,  //! has not yet been initialized
    Paused,    //! ready for another step
    Queued,    //! queued to run
    Running,   //! running in a work thread
    Finished,  //! all is done
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
template<>
inline QString _enumToString<PluginType>(PluginType type)
{
    switch (type) {
    case PluginType::Graph: return "Graph";
    case PluginType::Model: return "Model";
    default: return "invalid";
    }
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
template<>
inline QString _enumToString<GraphType>(GraphType type)
{
    switch (type) {
    case GraphType::Undirected: return "undirected";
    case GraphType::Directed: return "directed";
    default: return "invalid";
    }
}

enum class Function : unsigned char {
    Invalid = 0,
    Min = 1,
    Max = 2,
    Rand = 3,
    Value = 4
};
template<>
inline Function _enumFromString<Function>(const QString& str) {
    if (str == "min") return Function::Min;
    if (str == "max") return Function::Max;
    if (str == "rand") return Function::Rand;
    if (str == "value") return Function::Value;
    return Function::Invalid;
}
template<>
inline QString _enumToString<Function>(Function func)
{
    switch (func) {
    case Function::Min: return "min";
    case Function::Max: return "max";
    case Function::Rand: return "rand";
    case Function::Value: return "value";
    default: return "invalid";
    }
}

} // evoplex
#endif // ENUM_H
