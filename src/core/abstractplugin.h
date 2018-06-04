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

#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QJsonObject>
#include <QString>

#include "attributes.h"
#include "valuespace.h"

namespace evoplex {
class AbstractPlugin
{
public:
    enum PluginType {
        GraphPlugin,
        ModelPlugin
    };

    explicit AbstractPlugin(const QJsonObject* metaData, const QString& libPath);
    virtual ~AbstractPlugin();

    inline const bool isValid() const { return m_isValid; }

    inline const QString& path() const { return m_libPath; }
    inline const PluginType type() const { return m_type; }
    inline const QString& id() const { return m_id; }
    inline const QString& author() const { return m_author; }
    inline const QString& name() const { return m_name; }
    inline const QString& description() const { return m_descr; }

    inline const std::vector<QString>& pluginAttrNames() const { return m_pluginAttrNames; }
    inline const AttributesSpace& pluginAttrSpace() const { return m_pluginAttrSpace; }

protected:
    bool m_isValid;
    bool attrsSpace(const QJsonObject* metaData, const QString& name,
                    AttributesSpace &space, std::vector<QString>& keys) const;

private:
    const QString m_libPath;
    PluginType m_type;
    QString m_id;
    QString m_author;
    QString m_name;
    QString m_descr;
    AttributesSpace m_pluginAttrSpace;
    std::vector<QString> m_pluginAttrNames;
};
}
#endif // ABSTRACTPLUGIN_H
