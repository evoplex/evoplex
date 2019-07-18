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

#ifndef GRAPHINPUTS_H
#define GRAPHINPUTS_H

#include "attributes.h"
#include "mainapp.h"
#include "output.h"

namespace evoplex {

class GraphInputs;
using GraphInputsPtr = std::unique_ptr<GraphInputs>;

class GraphInputs
{
public:

    // We assume that all graph/model attributes start with 'uid_'. It is very
    // important to avoid clashes between different attributes which use the same name.
    // @return nullptr if doesn't have valid plugins
    static GraphInputsPtr parse(const MainApp* mainApp, const QStringList& header,
        const QStringList& values, QString& errMsg);

    ~GraphInputs();

    inline const GraphPlugin* graphPlugin() const;

    inline const Attributes* general() const;
    inline const Attributes* model() const;
    inline const Attributes* graph() const;

    inline Value general(const QString& name) const;
    inline Value model(const QString& name) const;
    inline Value graph(const QString& name) const;
    inline const std::vector<Cache*>& fileCaches() const;

    // Export all the attributes' names to a vector.
    // prefix all model's attributes with the modelId
    // prefix all graph's attributes with the graphId
    std::vector<QString> exportAttrNames(bool appendVersion) const;

    // Export all the attributes' values to a vector.
    std::vector<Value> exportAttrValues() const;

private:
    explicit GraphInputs(const GraphPlugin* g, Attributes* general, Attributes* graph, const std::vector<Cache*>& caches);

    const GraphPlugin* m_graphPlugin;

    Attributes* m_generalAttrs;
    Attributes* m_graphAttrs;
    Attributes* m_modelAttrs;

    std::vector<Cache*> m_fileCaches;

    static Plugin* findPlugin(PluginType type, const MainApp* mainApp,
        const QStringList& header, const QStringList& values, QString& errMsg);

    static void parseAttrs(GraphInputs* gi, const MainApp* mainApp,
        const QStringList& header, const QStringList& values, QStringList& failedAttrs);
};

inline const GraphPlugin* GraphInputs::graphPlugin() const
{
    return m_graphPlugin;
}

inline const Attributes* GraphInputs::general() const
{
    return m_generalAttrs;
}

inline const Attributes* GraphInputs::model() const
{
    return m_modelAttrs;
}

inline const Attributes* GraphInputs::graph() const
{
    return m_graphAttrs;
}

inline Value GraphInputs::general(const QString& name) const
{
    return m_generalAttrs->value(name);
}

inline Value GraphInputs::model(const QString& name) const
{
    return m_modelAttrs->value(name);
}

inline Value GraphInputs::graph(const QString& name) const
{
    return m_graphAttrs->value(name);
}

inline const std::vector<Cache*>& GraphInputs::fileCaches() const
{
    return m_fileCaches;
}

} // evoplex
#endif // GRAPHINPUTS_H
