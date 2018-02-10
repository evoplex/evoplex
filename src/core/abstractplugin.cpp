/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QJsonArray>
#include <QVariantMap>

#include "abstractplugin.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

AbstractPlugin::AbstractPlugin(const QJsonObject* metaData)
    : m_isValid(false)
{
    QString t = metaData->value(PLUGIN_ATTRIBUTE_TYPE).toString();
    if (t == "graph") {
        m_type = GraphPlugin;
    } else if (t == "model") {
        m_type = ModelPlugin;
    } else {
        return;
    }

    m_id = metaData->value(PLUGIN_ATTRIBUTE_UID).toString();
    m_author = metaData->value(PLUGIN_ATTRIBUTE_AUTHOR).toString();
    m_name = metaData->value(PLUGIN_ATTRIBUTE_NAME).toString();
    m_descr = metaData->value(PLUGIN_ATTRIBUTE_DESCRIPTION).toString();

    if (m_id.isEmpty() || m_author.isEmpty() || m_name.isEmpty() || m_descr.isEmpty()
            || !attrsSpace(metaData, PLUGIN_ATTRIBUTE_SPACE, m_pluginAttrSpace, m_pluginAttrNames)) {
        qWarning() << "[AbstractPlugin]: failed to read the plugins's attributes!";
        m_isValid = false;
        return;
    }

    m_isValid = true;
}

AbstractPlugin::~AbstractPlugin()
{
    Utils::deleteAndShrink(m_pluginAttrSpace);
}

bool AbstractPlugin::attrsSpace(const QJsonObject* metaData, const QString& name,
                                AttributesSpace& space, std::vector<QString>& keys) const
{
    if (metaData->contains(name)) {
        QJsonArray json = metaData->value(name).toArray();
        for (int id = 0; id < json.size(); ++id) {
            QVariantMap attrs = json.at(id).toObject().toVariantMap();
            ValueSpace* valSpace = ValueSpace::parse(id, attrs.firstKey(), attrs.first().toString());
            if (!valSpace->isValid()) {
                delete valSpace;
                Utils::deleteAndShrink(space);
                Utils::deleteAndShrink(keys);
                return false;
            }
            space.insert(valSpace->attrName(), valSpace);
            keys.push_back(valSpace->attrName());
        }
    }
    return true;
}

} // evoplex
