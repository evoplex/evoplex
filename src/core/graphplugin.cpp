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

#include <QDebug>
#include <QJsonArray>

#include "graphplugin.h"
#include "constants.h"

namespace evoplex {

GraphPlugin::GraphPlugin(const QJsonObject* metaData, const QString& libPath)
    : Plugin(PluginType::Graph, metaData, libPath)
{
    if (m_type == PluginType::Invalid) {
        return;
    }

    if (!metaData->contains(PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES)) {
        qWarning() << "missing 'validGraphTypes'.";
        m_type = PluginType::Invalid;
        return;
    }

    QJsonArray array = metaData->value(PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES).toArray();
    for (auto it : array) {
        GraphType type = _enumFromString<GraphType>(it.toString());
        if (type == GraphType::Invalid) {
            qWarning() << "invalid value for 'validGraphTypes':" << it.toString();
            m_type = PluginType::Invalid;
            return;
        }
        m_validGraphTypes.emplace_back(type);
    }
}

} // evoplex
