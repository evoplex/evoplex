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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <vector>

#include <QJsonObject>
#include <QString>
#include <QPluginLoader>

#include "abstractplugin.h"
#include "attributes.h"
#include "attributerange.h"
#include "plugininterface.h"

namespace evoplex {

using PluginKey = std::pair<QString, quint16>;  // <id, version>

class Plugin
{
public:
    virtual ~Plugin();

    // converts a key pair to a formated string
    static QString keyStr(const PluginKey& key);

    static Plugin* load(const QString& path, QString& error);

    inline AbstractPlugin* create() const;

    // The plugin's key is used to identify this plugin internally
    inline const PluginKey& key() const;

    inline const QString& path() const;
    inline PluginType type() const;
    inline const QString& id() const;
    inline const QString& author() const;
    inline const QString& title() const;
    inline const QString& description() const;
    inline quint16 version() const;
    inline QJsonObject metaData() const;
    inline const QString& compactMetaData() const;

    inline const std::vector<QString>& pluginAttrsNames() const;
    inline const AttributesScope& pluginAttrsScope() const;
    inline const AttributeRangePtr pluginAttrRange(const QString& attr) const;

protected:
    PluginType m_type;
    QJsonObject m_metaData;

    /**
     * Assumes that any plugin is loaded from the temporary directory
     * @param type plugin type
     * @param loader
     * @param libPath refers to the original plugin's path
     */
    explicit Plugin(PluginType type, QPluginLoader* loader, const QString& libPath);

    bool readAttrsScope(const QString& attrName, AttributesScope& attrsScope,
                        std::vector<QString>& keys) const;

private:
    QPluginLoader* m_loader;
    PluginInterface* m_factory;
    const QString m_libPath;
    QString m_compactMetaData;
    PluginKey m_key;
    QString m_id;
    QString m_author;
    QString m_title;
    QString m_descr;
    quint16 m_version;
    AttributesScope m_pluginAttrsScope;
    std::vector<QString> m_pluginAttrsNames;

    static bool checkMetaData(const QJsonObject& metaData, QString& error);
};

inline AbstractPlugin* Plugin::create() const
{ return m_factory->create(); }

inline const PluginKey& Plugin::key() const
{ return m_key; }

inline const QString& Plugin::path() const
{ return m_libPath; }

inline PluginType Plugin::type() const
{ return m_type; }

inline const QString& Plugin::id() const
{ return m_id; }

inline const QString& Plugin::author() const
{ return m_author; }

inline const QString& Plugin::title() const
{ return m_title; }

inline const QString& Plugin::description() const
{ return m_descr; }

inline quint16 Plugin::version() const
{ return m_version; }

inline QJsonObject Plugin::metaData() const
{ return m_metaData; }

inline const QString& Plugin::compactMetaData() const
{ return m_compactMetaData; }

inline const std::vector<QString>& Plugin::pluginAttrsNames() const
{ return m_pluginAttrsNames; }

inline const AttributesScope& Plugin::pluginAttrsScope() const
{ return m_pluginAttrsScope; }

inline const AttributeRangePtr Plugin::pluginAttrRange(const QString& attr) const
{ return m_pluginAttrsScope.value(attr); }

} // evoplex
// makes PluginType available to QMetaType system
Q_DECLARE_METATYPE(evoplex::PluginKey)

#endif // PLUGIN_H
