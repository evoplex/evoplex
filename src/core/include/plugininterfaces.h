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

#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

#include <QObject>
#include <QtPlugin>

#include "abstractgraph.h"
#include "abstractmodel.h"

namespace evoplex {

class IPluginModel
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginModel() {}

    // create the real model object.
    virtual AbstractModel* create() = 0;
};

class IPluginGraph
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginGraph() {}

    // create the real graph object.
    virtual AbstractGraph* create() = 0;
};
}

Q_DECLARE_INTERFACE(evoplex::IPluginGraph, "org.evoplex.IPluginGraph")

Q_DECLARE_INTERFACE(evoplex::IPluginModel, "org.evoplex.IPluginModel")


#define REGISTER_MODEL(CLASSNAME)                                           \
    namespace evoplex {                                                     \
    class PM_##CLASSNAME: public QObject, public IPluginModel               \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginModel"                        \
                      FILE "metadata.json")                                 \
    Q_INTERFACES(evoplex::IPluginModel)                                     \
    public:                                                                 \
        AbstractModel* create() {                                           \
            return dynamic_cast<AbstractModel*>(new CLASSNAME());           \
        }                                                                   \
    };}

#define REGISTER_GRAPH(CLASSNAME)                                           \
    namespace evoplex {                                                     \
    class PG_##CLASSNAME: public QObject, public IPluginGraph               \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginGraph"                        \
                      FILE "metadata.json")                                 \
    Q_INTERFACES(evoplex::IPluginGraph)                                     \
    public:                                                                 \
        AbstractGraph* create() {                                           \
            return dynamic_cast<AbstractGraph*>(new CLASSNAME(#CLASSNAME)); \
        }                                                                   \
    };}

#endif // PLUGININTERFACES_H
