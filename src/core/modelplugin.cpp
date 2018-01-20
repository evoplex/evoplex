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

    if (!attrsSpace(metaData, PLUGIN_ATTRIBUTE_MODELSPACE, m_modelAttrSpace, m_modelAttrNames)) {
        qWarning() << "[ModelPlugin]: failed to read the model's attributes!";
        m_isValid = false;
        return;
    }

    m_isValid = true;
}

ModelPlugin::~ModelPlugin()
{
    qDeleteAll(m_agentAttrSpace);
    qDeleteAll(m_edgeAttrSpace);
    qDeleteAll(m_modelAttrSpace);
}

} // evoplex
