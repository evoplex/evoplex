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
    explicit ModelPlugin(QPluginLoader* loader, const QString& libPath);

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
