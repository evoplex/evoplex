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

    inline AbstractPlugin* create() const { return m_factory->create(); }

    // The plugin's key is used to identify this plugin internally
    inline const PluginKey& key() const { return m_key; }

    inline const QString& path() const { return m_libPath; }
    inline PluginType type() const { return m_type; }
    inline const QString& id() const { return m_id; }
    inline const QString& author() const { return m_author; }
    inline const QString& title() const { return m_title; }
    inline const QString& description() const { return m_descr; }
    inline quint16 version() const { return m_version; }

    inline const std::vector<QString>& pluginAttrsNames() const { return m_pluginAttrsNames; }
    inline const AttributesScope& pluginAttrsScope() const { return m_pluginAttrsScope; }
    inline const AttributeRange* pluginAttrRange(const QString& attr) const { return m_pluginAttrsScope.value(attr); }

protected:
    PluginType m_type;

    explicit Plugin(PluginType type, const QJsonObject* metaData, const QString& libPath);

    bool readAttrsScope(const QJsonObject* metaData, const QString& attrName,
            AttributesScope& attrsScope, std::vector<QString>& keys) const;

private:
    PluginInterface* m_factory;
    const QString m_libPath;
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
} // evoplex

// makes PluginType available to QMetaType system
Q_DECLARE_METATYPE(evoplex::PluginKey)

#endif // PLUGIN_H
