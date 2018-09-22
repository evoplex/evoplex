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

#include "modelplugin.h"
#include "plugin.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

ModelPlugin::ModelPlugin(QPluginLoader* loader, const QString& libPath)
    : Plugin(PluginType::Model, loader, libPath)
{
    if (m_type == PluginType::Invalid) {
        return;
    }

    if (!readAttrsScope(PLUGIN_ATTR_NODESCOPE, m_nodeAttrsScope, m_nodeAttrNames)) {
        qWarning() << "failed to read the node's attributes!";
        m_type = PluginType::Invalid;
        return;
    }

    if (!readAttrsScope(PLUGIN_ATTR_EDGESCOPE, m_edgeAttrsScope, m_edgeAttrNames)) {
        qWarning() << "failed to read the edge's attributes!";
        m_type = PluginType::Invalid;
        return;
    }

    QJsonArray supportedGraphs = m_metaData.value(PLUGIN_ATTR_SUPPORTEDGRAPHS).toArray();
    m_supportedGraphs.reserve(supportedGraphs.size());
    for (QJsonValueRef v : supportedGraphs) {
        QString graphId = v.toString();
        if(!graphId.isEmpty()) {
            m_supportedGraphs.push_back(graphId);
        }
    }

    QJsonArray customOutputs = m_metaData.value(PLUGIN_ATTR_CUSTOMOUTPUTS).toArray();
    m_customOutputs.reserve(customOutputs.size());
    for (QJsonValueRef v : customOutputs) {
        QString function = v.toString();
        if(!function.isEmpty()) {
            m_customOutputs.push_back(function);
        }
    }
}

ModelPlugin::~ModelPlugin()
{
}

} // evoplex
