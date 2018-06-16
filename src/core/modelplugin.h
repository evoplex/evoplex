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

#ifndef MODELPLUGIN_H
#define MODELPLUGIN_H

#include <QVector>

#include "abstractplugin.h"
#include "attributes.h"
#include "plugininterfaces.h"

namespace evoplex {
class ModelPlugin : public AbstractPlugin
{
public:
    ModelPlugin(const QObject* instance, const QJsonObject* metaData, const QString& libPath);
    virtual ~ModelPlugin();

    AbstractModel* create() const { return m_factory->create(); }

    inline const QVector<QString>& customOutputs() const { return m_customOutputs; }
    inline const QVector<QString>& supportedGraphs() const { return m_supportedGraphs; }
    inline bool graphIsSupported(const QString& graphId) const { return m_supportedGraphs.empty() || m_supportedGraphs.contains(graphId); }

    inline const std::vector<QString>& nodeAttrNames() const { return m_nodeAttrNames; }
    inline const AttributesSpace& nodeAttrSpace() const { return m_nodeAttrSpace; }
    inline const ValueSpace* nodeAttrSpace(const QString& attr) const { return m_nodeAttrSpace.value(attr); }

    inline const std::vector<QString>& edgeAttrNames() const { return m_edgeAttrNames; }
    inline const AttributesSpace& edgeAttrSpace() const { return m_edgeAttrSpace; }
    inline const ValueSpace* edgeAttrSpace(const QString& attr) const { return m_edgeAttrSpace.value(attr); }

private:
    IPluginModel* m_factory;

    QVector<QString> m_supportedGraphs;
    QVector<QString> m_customOutputs;

    AttributesSpace m_nodeAttrSpace;
    std::vector<QString> m_nodeAttrNames;

    AttributesSpace m_edgeAttrSpace;
    std::vector<QString> m_edgeAttrNames;
};
}

#endif // MODELPLUGIN_H
