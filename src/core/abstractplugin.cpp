/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QJsonArray>
#include <QVariantMap>

#include "abstractplugin.h"
#include "constants.h"

namespace evoplex {

AbstractPlugin::AbstractPlugin(const QJsonObject* metaData)
    : m_isValid(false)
{
    m_id = metaData->value(PLUGIN_ATTRIBUTE_UID).toString();
    m_author = metaData->value(PLUGIN_ATTRIBUTE_AUTHOR).toString();
    m_name = metaData->value(PLUGIN_ATTRIBUTE_NAME).toString();
    m_descr = metaData->value(PLUGIN_ATTRIBUTE_DESCRIPTION).toString();

    m_isValid = !(m_id.isEmpty() || m_author.isEmpty() || m_name.isEmpty() || m_descr.isEmpty());
}

AttributesSpace AbstractPlugin::attrsSpace(const QJsonObject* metaData, const QString& name) const
{
    AttributesSpace ret;
    if (metaData->contains(name)) {
        QJsonArray json = metaData->value(name).toArray();
        for (int i = 0; i < json.size(); ++i) {
            QVariantMap attrs = json.at(i).toObject().toVariantMap();
            ret.insert(attrs.firstKey(), qMakePair(i, attrs.first().toString()));
        }
    }
    return ret;
}

} // evoplex
