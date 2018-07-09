/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include "abstractgraph.h"
#include "attributes.h"
#include "plugin.h"

namespace evoplex {
class GraphPlugin : public Plugin
{
    friend class Plugin;
    friend class MainApp;

public:
    using GraphTypes = std::vector<AbstractGraph::GraphType>;

    virtual ~GraphPlugin() = default;

    inline const GraphTypes& validGraphTypes() const;

protected:
    explicit GraphPlugin(const QJsonObject* metaData, const QString& libPath);

private:
    std::vector<AbstractGraph::GraphType> m_validGraphTypes;
};

/************************************************************************
   GraphPlugin: Inline member functions
 ************************************************************************/

inline const GraphPlugin::GraphTypes& GraphPlugin::validGraphTypes() const
{ return m_validGraphTypes; }

} //evoplex
#endif // GRAPHPLUGIN_H
