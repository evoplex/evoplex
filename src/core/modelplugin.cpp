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
#include "abstractplugin.h"
#include "utils.h"

namespace evoplex {

ModelPlugin::ModelPlugin(const QObject* instance, const QJsonObject* metaData, const QString& libPath)
    : AbstractPlugin(metaData, libPath)
{
    if (!isValid()) {
        return;
    }

    m_factory = qobject_cast<IPluginModel*>(instance);

    if (!attrsSpace(metaData, PLUGIN_ATTRIBUTE_AGENTSPACE, m_agentAttrSpace, m_agentAttrNames)) {
        qWarning() << "[ModelPlugin]: failed to read the agent's attributes!";
        m_isValid = false;
        return;
    }

    if (!attrsSpace(metaData, PLUGIN_ATTRIBUTE_EDGESPACE, m_edgeAttrSpace, m_edgeAttrNames)) {
        qWarning() << "[ModelPlugin]: failed to read the edge's attributes!";
        m_isValid = false;
        return;
    }

    for (QJsonValueRef v : metaData->value(PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS).toArray()) {
        QString graphId = v.toString();
        if(!graphId.isEmpty()) {
            m_supportedGraphs.push_back(graphId);
        }
    }

    for (QJsonValueRef v : metaData->value(PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS).toArray()) {
        QString function = v.toString();
        if(!function.isEmpty()) {
            m_customOutputs.push_back(function);
        }
    }

    m_isValid = true;
}

ModelPlugin::~ModelPlugin()
{
    Utils::deleteAndShrink(m_agentAttrSpace);
    Utils::deleteAndShrink(m_edgeAttrSpace);
}

} // evoplex
