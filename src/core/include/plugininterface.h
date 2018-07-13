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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>
#include <QtPlugin>

#include "abstractgraph.h"
#include "abstractmodel.h"

namespace evoplex {

class Trial;

class PluginInterface
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~PluginInterface() = default;

    // create a model/graph object.
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
