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
#include <QFile>
#include <QJsonArray>
#include <QPluginLoader>
#include <QVariantMap>

#include "plugin.h"
#include "graphplugin.h"
#include "modelplugin.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

Plugin::Type Plugin::enumFromString(const QString& type)
{
    if (type == "graph") return Graph;
    if (type == "model") return Model;
    return Invalid;
}

bool Plugin::checkMetaData(const QJsonObject& metaData, QString& error)
{
    if (metaData.isEmpty()) {
        error = "Meta data is missing.";
        return false;
    }

    const QStringList reqFields = {
        PLUGIN_ATTRIBUTE_TYPE, PLUGIN_ATTRIBUTE_UID, PLUGIN_ATTRIBUTE_NAME,
        PLUGIN_ATTRIBUTE_AUTHOR, PLUGIN_ATTRIBUTE_DESCRIPTION
    };

    for (const QString& f : reqFields) {
        if (!metaData.contains(f)) {
            error = QString("The following fields cannot be empty: %1")
                    .arg(reqFields.join(", "));
            return false;
        }
    }

    const Type type = enumFromString(metaData[PLUGIN_ATTRIBUTE_TYPE].toString());
    if (type == Invalid) {
        error = QString("'%1' must be equal to 'graph' or 'model'")
                .arg(PLUGIN_ATTRIBUTE_TYPE);
        return false;
    }

    const QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (uid.contains("_")) {
        error = QString("The %1 '%2' should not have the underscore symbol.")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(uid);
        return false;
    }

    return true;
}

Plugin* Plugin::load(const QString& path, QString& error)
{
    if (!QFile(path).exists()) {
        error = "Unable to find the file. " + path;
        qWarning() << error;
        return nullptr;
    }

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData().value("MetaData").toObject();
    if (!checkMetaData(metaData, error)) {
        error += QString("\n+%1").arg(path);
        qWarning() << error;
        return nullptr;
    }

    QObject* instance = loader.instance(); // it'll load the plugin
    if (!instance) {
        error = QString("Unable to load the plugin.\n"
                "Please, make sure it is a valid Evoplex plugin.\n %1").arg(path);
        loader.unload();
        qWarning() << error;
        return nullptr;
    }

    Plugin* plugin = nullptr;
    const Type type = enumFromString(metaData[PLUGIN_ATTRIBUTE_TYPE].toString());
    if (type == Graph) {
        plugin = new GraphPlugin(&metaData, path);
    } else if (type == Model) {
        plugin = new ModelPlugin(&metaData, path);
    }

    if (!plugin || plugin->type() == Plugin::Invalid) {
        error = QString("Unable to load the plugin.\n"
                "Please, check the metaData.json file.\n %1").arg(path);
        loader.unload();
        delete plugin;
        qWarning() << error;
        return nullptr;
    }

    return plugin;
}

Plugin::Plugin(Type type, const QJsonObject* metaData, const QString& libPath)
    : m_type(type),
      m_libPath(libPath)
{
    m_id = metaData->value(PLUGIN_ATTRIBUTE_UID).toString();
    m_author = metaData->value(PLUGIN_ATTRIBUTE_AUTHOR).toString();
    m_name = metaData->value(PLUGIN_ATTRIBUTE_NAME).toString();
    m_descr = metaData->value(PLUGIN_ATTRIBUTE_DESCRIPTION).toString();

    Q_ASSERT_X(!m_id.isEmpty() && !m_author.isEmpty() &&
               !m_name.isEmpty() && !m_descr.isEmpty(),
               "Plugin", "missing required fields! It should never happen!");

    if (!readAttrsScope(metaData, PLUGIN_ATTRIBUTES_SCOPE,
                        m_pluginAttrsScope, m_pluginAttrsNames)) {
        qWarning() << "failed to read the plugins's attributes!";
        m_type = Invalid;
        return;
    }

    QPluginLoader loader(libPath);
    QObject* instance = loader.instance();
    Q_ASSERT(loader.isLoaded() && instance);
    m_factory = qobject_cast<PluginInterface*>(instance);
    Q_ASSERT_X(m_factory, "Plugin", qPrintable(QString("factory could not be created for '%1'").arg(m_name)));
}

Plugin::~Plugin()
{
    QPluginLoader loader(m_libPath);
    loader.unload();
    Utils::deleteAndShrink(m_pluginAttrsScope);
}

bool Plugin::readAttrsScope(const QJsonObject* metaData, const QString& name,
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
