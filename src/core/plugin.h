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
     * Relies that any plugin lugin is loaded from the temporary directory
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
