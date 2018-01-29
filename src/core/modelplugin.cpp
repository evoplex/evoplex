/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QJsonArray>

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
    qDeleteAll(m_agentAttrSpace);
    qDeleteAll(m_edgeAttrSpace);
}

} // evoplex
