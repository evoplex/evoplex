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
#include <QVariantMap>

#include "plugin.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

Plugin::Plugin(const QJsonObject* metaData, const QString& libPath)
    : m_isValid(false)
    , m_libPath(libPath)
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
            || !attrsScope(metaData, PLUGIN_ATTRIBUTES_SCOPE, m_pluginAttrsScope, m_pluginAttrsNames)) {
        qWarning() << "failed to read the plugins's attributes!";
        m_isValid = false;
        return;
    }

    m_isValid = true;
}

Plugin::~Plugin()
{
    Utils::deleteAndShrink(m_pluginAttrsScope);
}

bool Plugin::attrsScope(const QJsonObject* metaData, const QString& name,
        AttributesScope& attrsScope, std::vector<QString>& keys) const
{
    if (metaData->contains(name)) {
        QJsonArray json = metaData->value(name).toArray();
        for (int id = 0; id < json.size(); ++id) {
            QVariantMap attrs = json.at(id).toObject().toVariantMap();
            AttributeRange* attrRange = AttributeRange::parse(id, attrs.firstKey(), attrs.first().toString());
            if (!attrRange->isValid()) {
                delete attrRange;
                Utils::deleteAndShrink(attrsScope);
                Utils::deleteAndShrink(keys);
                return false;
            }
            attrsScope.insert(attrRange->attrName(), attrRange);
            keys.push_back(attrRange->attrName());
        }
    }
    return true;
}

} // evoplex
