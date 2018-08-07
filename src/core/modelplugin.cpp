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

#include "modelplugin.h"
#include "plugin.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

ModelPlugin::ModelPlugin(const QJsonObject* metaData, const QString& libPath)
    : Plugin(PluginType::Model, metaData, libPath)
{
    if (m_type == PluginType::Invalid) {
        return;
    }

    if (!readAttrsScope(metaData, PLUGIN_ATTR_NODESCOPE, m_nodeAttrsScope, m_nodeAttrNames)) {
        qWarning() << "failed to read the node's attributes!";
        m_type = PluginType::Invalid;
        return;
    }

    if (!readAttrsScope(metaData, PLUGIN_ATTR_EDGESCOPE, m_edgeAttrsScope, m_edgeAttrNames)) {
        qWarning() << "failed to read the edge's attributes!";
        m_type = PluginType::Invalid;
        return;
    }

    QJsonArray supportedGraphs = metaData->value(PLUGIN_ATTR_SUPPORTEDGRAPHS).toArray();
    m_supportedGraphs.reserve(supportedGraphs.size());
    for (QJsonValueRef v : supportedGraphs) {
        QString graphId = v.toString();
        if(!graphId.isEmpty()) {
            m_supportedGraphs.push_back(graphId);
        }
    }

    QJsonArray customOutputs = metaData->value(PLUGIN_ATTR_CUSTOMOUTPUTS).toArray();
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
