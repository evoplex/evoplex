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
