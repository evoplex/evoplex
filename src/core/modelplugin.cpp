/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "modelplugin.h"
#include "abstractplugin.h"
#include "utils.h"

namespace evoplex {

ModelPlugin::ModelPlugin(const QObject* instance, const QJsonObject* metaData)
    : AbstractPlugin(metaData)
{
    if (!isValid()) {
        return;
    }

    m_factory = qobject_cast<IPluginModel*>(instance);
    m_supportedGraphs = metaData->value(PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS).toString().split(",").toVector();
    m_customOutputs = metaData->value(PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS).toString().split(",").toVector();

    m_agentAttrSpace = attrsSpace(metaData, PLUGIN_ATTRIBUTE_AGENTSPACE);
    if (!m_agentAttrSpace.isEmpty() && !Utils::boundaryValues(m_agentAttrSpace, m_agentAttrRange)) {
        qWarning() << "[ModelPlugin]: failed to read the agent's attributes!";
        m_isValid = false;
        return;
    }

    m_edgeAttrSpace = attrsSpace(metaData, PLUGIN_ATTRIBUTE_EDGESPACE);
    if (!m_edgeAttrSpace.isEmpty() && !Utils::boundaryValues(m_edgeAttrSpace, m_edgeAttrRange)) {
        qWarning() << "[ModelPlugin]: failed to read the edge's attributes!";
        m_isValid = false;
        return;
    }

    m_modelAttrSpace = attrsSpace(metaData, PLUGIN_ATTRIBUTE_MODELSPACE);
    if (!m_modelAttrSpace.isEmpty() && !Utils::boundaryValues(m_modelAttrSpace, m_modelAttrRange)) {
        qWarning() << "[ModelPlugin]: failed to read the model's attributes!";
        m_isValid = false;
        return;
    }

    m_isValid = true;
}

} // evoplex
