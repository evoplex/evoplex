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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>
#include <QtPlugin>

#include "abstractgraph.h"
#include "abstractmodel.h"

namespace evoplex {

/**
 * @brief A common interface for plugins.
 */
class PluginInterface
{
public:
    //! Provides the destructor to keep compilers happy.
    virtual ~PluginInterface() = default;

    //! Creates a model/graph object.
    virtual AbstractPlugin* create() = 0;
};
}

Q_DECLARE_INTERFACE(evoplex::PluginInterface, "org.evoplex.PluginInterface")

#define REGISTER_PLUGIN(CLASSNAME)                                   \
    namespace evoplex {                                              \
    class PG_##CLASSNAME : public QObject, public PluginInterface    \
    {                                                                \
    Q_OBJECT                                                         \
    Q_PLUGIN_METADATA(IID "org.evoplex.PluginInterface"              \
                      FILE "metadata.json")                          \
    Q_INTERFACES(evoplex::PluginInterface)                           \
    public:                                                          \
        AbstractPlugin* create() { return new CLASSNAME(); }         \
    };}

#endif // PLUGININTERFACE_H
