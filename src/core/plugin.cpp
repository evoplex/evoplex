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
#include <QDir>
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

QString Plugin::keyStr(const PluginKey& key)
{
    return QString("%1-%2").arg(key.first).arg(key.second);
}

bool Plugin::checkMetaData(const QJsonObject& metaData, QString& error)
{
    if (metaData.isEmpty()) {
        error = "Meta data is missing.";
        return false;
    }

    const QStringList reqFields = {
        PLUGIN_ATTR_TYPE, PLUGIN_ATTR_UID, PLUGIN_ATTR_TITLE,
        PLUGIN_ATTR_AUTHOR, PLUGIN_ATTR_DESCRIPTION, PLUGIN_ATTR_VERSION
    };

    for (const QString& f : reqFields) {
        if (!metaData.contains(f)) {
            error = QString("The following fields cannot be empty: %1")
                    .arg(reqFields.join(", "));
            return false;
        }
    }

    const PluginType type = _enumFromString<PluginType>(metaData[PLUGIN_ATTR_TYPE].toString());
    if (type == PluginType::Invalid) {
        error = QString("'%1' must be equal to 'graph' or 'model'")
                .arg(PLUGIN_ATTR_TYPE);
        return false;
    }

    const QString uid = metaData[PLUGIN_ATTR_UID].toString();
    if (uid.contains("_")) {
        error = QString("The %1 '%2' should not have the underscore symbol.")
                .arg(PLUGIN_ATTR_UID, uid);
        return false;
    }

    return true;
}

Plugin* Plugin::load(const QString& path, QString& error)
{
    QFile file(path);
    if (!file.exists()) {
        error = "Unable to find the file. " + path;
        qWarning() << error;
        return nullptr;
    }

    // we should NEVER load the plugin from the original path
    // instead, we make a copy of the plugin to the temporary dir
    // it allows us to safely unload and reload plugins later on
    QString tempPath = path;
    QString fname = QFileInfo(file).fileName();
    while (QFile::exists(tempPath)) {
        tempPath = QDir::temp().absoluteFilePath(
                QString("evoplex_%1%2").arg(rand()).arg(fname));
    }
    if (!file.copy(tempPath)) {
        error = "Unable make a temporary copy of the file " + path;
        qWarning() << error;
        return nullptr;
    }

    QPluginLoader* loader = new QPluginLoader(tempPath);
    QJsonObject metaData = loader->metaData().value("MetaData").toObject();
    if (!checkMetaData(metaData, error)) {
        error += QString("\n+%1").arg(path);
        qWarning() << error;
        return nullptr;
    }

    QObject* instance = loader->instance(); // it'll load the plugin
    if (!instance) {
        error = QString("Unable to load the plugin.\n"
                "Please, make sure it is a valid Evoplex plugin and that it was"
                " built in the same mode (Release or Debug) and architecture "
                "(32/64 bits) of Evoplex.\n %1").arg(path);
        loader->unload();
        qWarning() << error;
        return nullptr;
    }

    Plugin* plugin = nullptr;
    const PluginType type = _enumFromString<PluginType>(metaData[PLUGIN_ATTR_TYPE].toString());
    if (type == PluginType::Graph) {
        plugin = new GraphPlugin(loader, path);
    } else if (type == PluginType::Model) {
        plugin = new ModelPlugin(loader, path);
    }

    if (!plugin || plugin->type() == PluginType::Invalid) {
        error = QString("Unable to load the plugin.\n"
                "Please, check the metaData.json file.\n %1").arg(path);
        qWarning() << error;
        delete  plugin;
        return nullptr;
    }

    return plugin;
}

Plugin::Plugin(PluginType type, QPluginLoader* loader, const QString& libPath)
    : m_type(type),
      m_loader(loader),
      m_factory(nullptr),
      m_libPath(libPath)
{
    QJsonObject metaData = m_loader->metaData().value("MetaData").toObject();
    m_id = metaData.value(PLUGIN_ATTR_UID).toString();
    m_author = metaData.value(PLUGIN_ATTR_AUTHOR).toString();
    m_title = metaData.value(PLUGIN_ATTR_TITLE).toString();
    m_descr = metaData.value(PLUGIN_ATTR_DESCRIPTION).toString();
    if (m_id.isEmpty() || m_author.isEmpty() || m_title.isEmpty() || m_descr.isEmpty()) {
        qWarning() << "missing required fields!";
        m_type = PluginType::Invalid;
        return;
    }

    int version = metaData.value(PLUGIN_ATTR_VERSION).toInt(-1);
    if (version < 0 || version >= UINT16_MAX) {
        qWarning() << QString("plugin's version must be an int >=0 and <%1").arg(UINT16_MAX);
        m_type = PluginType::Invalid;
        return;
    }
    m_version = static_cast<quint16>(version);

    m_key = {m_id, m_version};

    if (!readAttrsScope(&metaData, PLUGIN_ATTR_ATTRSSCOPE,
                        m_pluginAttrsScope, m_pluginAttrsNames)) {
        qWarning() << "failed to read the plugins's attributes!";
        m_type = PluginType::Invalid;
        return;
    }

    m_factory = qobject_cast<PluginInterface*>(m_loader->instance());
    if (!m_factory) {
        qWarning() << QString("factory could not be created for '%1'").arg(m_title);
        m_type = PluginType::Invalid;
        return;
    }
}

Plugin::~Plugin()
{
    if (!m_loader->unload() || !QFile::remove(m_loader->fileName())) {
        qWarning() << "failed to remove the plugin";
    }
    delete m_loader;
}

bool Plugin::readAttrsScope(const QJsonObject* metaData, const QString& attrName,
        AttributesScope& attrsScope, std::vector<QString>& keys) const
{
    if (metaData->contains(attrName)) {
        QJsonArray json = metaData->value(attrName).toArray();
        for (int id = 0; id < json.size(); ++id) {
            QVariantMap attrs = json.at(id).toObject().toVariantMap();
            auto attrRange = AttributeRange::parse(id, attrs.firstKey(), attrs.first().toString());
            if (!attrRange->isValid()) {
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
