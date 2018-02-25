/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QJsonArray>

#include "graphplugin.h"
#include "abstractplugin.h"

namespace evoplex {

GraphPlugin::GraphPlugin(const QObject* instance, const QJsonObject* metaData, const QString& libPath)
    : AbstractPlugin(metaData, libPath)
{
    if (!isValid()) {
        return;
    }

    m_factory = qobject_cast<IPluginGraph*>(instance);

    if (!metaData->contains(PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES)) {
        qWarning() << "[GraphPlugin]: missing 'validGraphTypes'.";
        m_isValid = false;
        return;
    } else {
        QJsonArray arr = metaData->value(PLUGIN_ATTRIBUTE_VALIDGRAPHTYPES).toArray();
        QJsonArray::iterator it = arr.begin();
        for (it; it != arr.end(); ++it) {
            AbstractGraph::GraphType type = AbstractGraph::enumFromString((*it).toString());
            if (type == AbstractGraph::Invalid_Type) {
                qWarning() << "[GraphPlugin]: invalid value for 'validGraphTypes':" << (*it).toString();
                m_isValid = false;
                return;
            }
            m_validGraphTypes.emplace_back(type);
        }
    }

    m_isValid = true;
}

GraphPlugin::~GraphPlugin()
{
}

} // evoplex
