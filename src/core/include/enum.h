#ifndef ENUM_H
#define ENUM_H

#include <QMap>

namespace evoplex {

/**
 * We need an efficient and simple way to convert enums to strings (vice-versa).
 * There are many ways to do that, we have tried 'better-enums' and
 * other solutions involving more complex macros, but none worked well here.
 * The decision so far was to use a simple QMap to handle that. It is a naive solution
 * but it's easier to use and maintain (as long as we don't have big enums).
 */
#define ENUM_MAP(C, ...) static QMap<C, QString> C##_map(__VA_ARGS__);

enum class GraphType {
    Directed,
    Unrected
};
ENUM_MAP(GraphType, {{GraphType::Directed, "directed"}, {GraphType::Unrected, "undirected"}})


} // evoplex
#endif // ENUM_H
