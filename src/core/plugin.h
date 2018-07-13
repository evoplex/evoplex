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
class Plugin
{
public:
    static Plugin* load(const QString& path, QString& error);

    inline AbstractPlugin* create() const { return m_factory->create(); }

    inline const QString& path() const { return m_libPath; }
    inline PluginType type() const { return m_type; }
    inline const QString& id() const { return m_id; }
    inline const QString& author() const { return m_author; }
    inline const QString& name() const { return m_name; }
    inline const QString& description() const { return m_descr; }

    inline const std::vector<QString>& pluginAttrsNames() const { return m_pluginAttrsNames; }
    inline const AttributesScope& pluginAttrsScope() const { return m_pluginAttrsScope; }
    inline const AttributeRange* pluginAttrRange(const QString& attr) const { return m_pluginAttrsScope.value(attr); }

protected:
    PluginType m_type;

    explicit Plugin(PluginType type, const QJsonObject* metaData, const QString& libPath);
    virtual ~Plugin();

    bool readAttrsScope(const QJsonObject* metaData, const QString& name,
            AttributesScope& attrsScope, std::vector<QString>& keys) const;

private:
    PluginInterface* m_factory;
    const QString m_libPath;
    QString m_id;
    QString m_author;
    QString m_name;
    QString m_descr;
    AttributesScope m_pluginAttrsScope;
    std::vector<QString> m_pluginAttrsNames;

    static bool checkMetaData(const QJsonObject& metaData, QString& error);
};
} // evoplex
#endif // PLUGIN_H
