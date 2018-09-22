/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
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
      m_libPath(libPath),
      m_version(0)
{
    m_metaData = m_loader->metaData().value("MetaData").toObject();
    m_id = m_metaData.value(PLUGIN_ATTR_UID).toString();
    m_author = m_metaData.value(PLUGIN_ATTR_AUTHOR).toString();
    m_title = m_metaData.value(PLUGIN_ATTR_TITLE).toString();
    m_descr = m_metaData.value(PLUGIN_ATTR_DESCRIPTION).toString();
    if (m_id.isEmpty() || m_author.isEmpty() || m_title.isEmpty() || m_descr.isEmpty()) {
        qWarning() << "missing required fields!";
        m_type = PluginType::Invalid;
        return;
    }

    int version = m_metaData.value(PLUGIN_ATTR_VERSION).toInt(-1);
    if (version < 0 || version >= UINT16_MAX) {
        qWarning() << QString("plugin's version must be an int >=0 and <%1").arg(UINT16_MAX);
        m_type = PluginType::Invalid;
        return;
    }
    m_version = static_cast<quint16>(version);

    m_key = {m_id, m_version};

    if (!readAttrsScope(PLUGIN_ATTR_ATTRSSCOPE, m_pluginAttrsScope, m_pluginAttrsNames)) {
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

    // build the compactMetaData without the general keys
    auto metaData = m_metaData;
    QStringList removeKeys = {
        PLUGIN_ATTR_TYPE, PLUGIN_ATTR_UID, PLUGIN_ATTR_AUTHOR,
        PLUGIN_ATTR_TITLE, PLUGIN_ATTR_DESCRIPTION, PLUGIN_ATTR_VERSION };
    for (auto const& k : removeKeys) {
        metaData.remove(k);
    }
    if (!metaData.isEmpty()) {
        QJsonDocument doc(metaData);
        m_compactMetaData = doc.toJson(QJsonDocument::Indented);
    }
}

Plugin::~Plugin()
{
    if (!m_loader->unload() || !QFile::remove(m_loader->fileName())) {
        qWarning() << "failed to remove the plugin";
    }
    delete m_loader;
}

bool Plugin::readAttrsScope(const QString& attrName, AttributesScope& attrsScope,
                            std::vector<QString>& keys) const
{
    if (m_metaData.contains(attrName)) {
        QJsonArray json = m_metaData.value(attrName).toArray();
        for (int id = 0; id < json.size(); ++id) {
            QVariantMap attrs = json.at(id).toObject().toVariantMap();
            auto attrRange = AttributeRange::parse(id, attrs.firstKey(), attrs.first().toString());
            if (!attrRange->isValid()) {
                Utils::clearAndShrink(keys);
                return false;
            }
            attrsScope.insert(attrRange->attrName(), attrRange);
            keys.push_back(attrRange->attrName());
        }
    }
    return true;
}

} // evoplex
