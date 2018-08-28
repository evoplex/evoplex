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

GraphPlugin::GraphPlugin(QPluginLoader* loader, const QString& libPath)
    : Plugin(PluginType::Graph, loader, libPath),
      m_supportsEdgeAttrsGen(false)
{
    if (m_type == PluginType::Invalid) {
        return;
    }

    if (m_metaData.contains(PLUGIN_ATTR_VALIDGRAPHTYPES)) {
        if (!m_metaData.value(PLUGIN_ATTR_VALIDGRAPHTYPES).isArray()) {
            qWarning() << QString("the attribute '%1' must be an array.")
                          .arg(PLUGIN_ATTR_VALIDGRAPHTYPES);
            m_type = PluginType::Invalid;
            return;
        }
        m_supportsEdgeAttrsGen = m_metaData.value(
                    PLUGIN_ATTR_EDGEATTRSGEN).toBool();
    }

    if (m_metaData.contains(PLUGIN_ATTR_EDGEATTRSGEN)) {
        if (!m_metaData.value(PLUGIN_ATTR_EDGEATTRSGEN).isBool()) {
            qWarning() << QString("the attribute '%1' must be a boolean.")
                          .arg(PLUGIN_ATTR_EDGEATTRSGEN);
            m_type = PluginType::Invalid;
            return;
        }

        QJsonArray array = m_metaData.value(PLUGIN_ATTR_VALIDGRAPHTYPES).toArray();
        for (auto it : array) {
            GraphType type = _enumFromString<GraphType>(it.toString());
            if (type == GraphType::Invalid) {
                qWarning() << QString("invalid value for '%1': %2")
                              .arg(PLUGIN_ATTR_VALIDGRAPHTYPES, it.toString());
                m_type = PluginType::Invalid;
                return;
            }
            m_validGraphTypes.emplace_back(type);
        }
    }
}

} // evoplex
