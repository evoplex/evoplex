/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "graphplugin.h"
#include "abstractplugin.h"
#include "utils.h"

namespace evoplex {

GraphPlugin::GraphPlugin(const QObject* instance, const QJsonObject* metaData)
    : AbstractPlugin(metaData)
{
    if (!isValid()) {
        return;
    }

    m_factory = qobject_cast<IPluginGraph*>(instance);

    if (!attrsSpace(metaData, PLUGIN_ATTRIBUTE_GRAPHSPACE, m_graphAttrSpace)) {
        qWarning() << "[GraphPlugin]: failed to read the graph's attributes!";
        m_isValid = false;
        return;
    }

    if (!m_graphAttrSpace.contains(PLUGIN_ATTRIBUTE_GRAPH_TYPE)) {
        qWarning() << "[GraphPlugin]: missing 'graphType'.";
        m_isValid = false;
        return;
    }

    m_isValid = true;
}

GraphPlugin::~GraphPlugin()
{
    qDeleteAll(m_graphAttrSpace);
}

} // evoplex
