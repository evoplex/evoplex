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

#include "abstractmodel.h"
#include "attributes.h"
#include "plugin.h"

namespace evoplex {
class ModelPlugin : public Plugin
{
    friend class Plugin;
    friend class MainApp;

public:
    virtual ~ModelPlugin();

    // the supported graphIds for this model
    // if empty, any graph is supported
    inline const QVector<QString>& supportedGraphs() const;
    inline bool graphIsSupported(const QString& graphId) const;

    inline const QVector<QString>& customOutputs() const;

    inline const std::vector<QString>& nodeAttrNames() const;
    inline const AttributesScope& nodeAttrsScope() const;
    inline AttributeRangePtr nodeAttrRange(const QString& attr) const;

    inline const std::vector<QString>& edgeAttrNames() const;
    inline const AttributesScope& edgeAttrsScope() const;
    inline AttributeRangePtr edgeAttrRange(const QString& attr) const;

protected:
    explicit ModelPlugin(const QJsonObject* metaData, const QString& libPath);

private:
    QVector<QString> m_supportedGraphs;
    QVector<QString> m_customOutputs;

    AttributesScope m_nodeAttrsScope;
    std::vector<QString> m_nodeAttrNames;

    AttributesScope m_edgeAttrsScope;
    std::vector<QString> m_edgeAttrNames;
};

/************************************************************************
   ModelPlugin: Inline member functions
 ************************************************************************/

inline const QVector<QString>& ModelPlugin::customOutputs() const
{ return m_customOutputs; }

inline const QVector<QString>& ModelPlugin::supportedGraphs() const
{ return m_supportedGraphs; }

inline bool ModelPlugin::graphIsSupported(const QString& graphId) const
{ return m_supportedGraphs.empty() || m_supportedGraphs.contains(graphId); }

inline const std::vector<QString>& ModelPlugin::nodeAttrNames() const
{ return m_nodeAttrNames; }

inline const AttributesScope& ModelPlugin::nodeAttrsScope() const
{ return m_nodeAttrsScope; }

inline AttributeRangePtr ModelPlugin::nodeAttrRange(const QString& attr) const
{ return m_nodeAttrsScope.value(attr); }

inline const std::vector<QString>& ModelPlugin::edgeAttrNames() const
{ return m_edgeAttrNames; }

inline const AttributesScope& ModelPlugin::edgeAttrsScope() const
{ return m_edgeAttrsScope; }

inline AttributeRangePtr ModelPlugin::edgeAttrRange(const QString& attr) const
{ return m_edgeAttrsScope.value(attr); }

} //evoplex
#endif // MODELPLUGIN_H
