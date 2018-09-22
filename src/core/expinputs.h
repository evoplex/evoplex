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

#ifndef EXPINPUTS_H
#define EXPINPUTS_H

#include "attributes.h"
#include "mainapp.h"
#include "output.h"

namespace evoplex {

class ExpInputs;
using ExpInputsPtr = std::unique_ptr<ExpInputs>;

class ExpInputs
{
public:
    // Read and validate the experiment inputs.
    // We assume that all graph/model attributes start with 'uid_'. It is very
    // important to avoid clashes between different attributes which use the same name.
    // @return nullptr if doesn't have valid plugins
    static ExpInputsPtr parse(const MainApp* mainApp, const QStringList& header,
                              const QStringList& values, QString& errMsg);

    ~ExpInputs();

    inline const GraphPlugin* graphPlugin() const;
    inline const ModelPlugin* modelPlugin() const;

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
    explicit ExpInputs(const GraphPlugin* g, const ModelPlugin* m,
                       Attributes* general, Attributes* graph, Attributes* model,
                       const std::vector<Cache*>& caches);

    const GraphPlugin* m_graphPlugin;
    const ModelPlugin* m_modelPlugin;
    Attributes* m_generalAttrs;
    Attributes* m_graphAttrs;
    Attributes* m_modelAttrs;
    std::vector<Cache*> m_fileCaches;

    static Plugin* findPlugin(PluginType type, const MainApp* mainApp,
            const QStringList& header, const QStringList& values, QString& errMsg);

    static void parseAttrs(ExpInputs* ei, const MainApp* mainApp,
            const QStringList& header, const QStringList& values, QStringList& failedAttrs);

    static void parseFileCache(ExpInputs* ei, QStringList& failedAttrs, QString& errMsg);

    static void checkAttrCommands(ExpInputs* ei, QStringList& failedAttrs);
};

inline const GraphPlugin* ExpInputs::graphPlugin() const
{ return m_graphPlugin; }

inline const ModelPlugin* ExpInputs::modelPlugin() const
{ return m_modelPlugin; }

inline const Attributes* ExpInputs::general() const
{ return m_generalAttrs; }

inline const Attributes* ExpInputs::model() const
{ return m_modelAttrs; }

inline const Attributes* ExpInputs::graph() const
{ return m_graphAttrs; }

inline Value ExpInputs::general(const QString& name) const
{ return m_generalAttrs->value(name); }

inline Value ExpInputs::model(const QString& name) const
{ return m_modelAttrs->value(name); }

inline Value ExpInputs::graph(const QString& name) const
{ return m_graphAttrs->value(name); }

inline const std::vector<Cache*>& ExpInputs::fileCaches() const
{ return m_fileCaches; }

} // evoplex
#endif // EXPINPUTS_H
