/* Evoplex <https://evoplex.org>
* Copyright (C) 2016-present
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

#include "graphinputs.h"
#include "constants.h"
#include "graphplugin.h"

namespace evoplex {

GraphInputs::GraphInputs(const GraphPlugin* g, Attributes* general, Attributes* graph, const std::vector<Cache*>& caches)
    : m_graphPlugin(g),
    m_generalAttrs(general),
    m_graphAttrs(graph),
    m_fileCaches(caches)
{
}

GraphInputs::~GraphInputs()
{
    delete m_generalAttrs;
    delete m_graphAttrs;
    for (Cache* c : m_fileCaches) {
        c->deleteCache();
    }
}

std::vector<QString> GraphInputs::exportAttrNames(bool appendVersion) const
{
    std::vector<QString> names = m_generalAttrs->names();
    const QString graphId = m_generalAttrs->value(GENERAL_ATTR_GRAPHID, "").toQString();
    const QString graphVs = appendVersion ? "-" + m_generalAttrs->value(GENERAL_ATTR_GRAPHVS, "").toQString() : "";
    for (const QString& attrName : m_graphAttrs->names()) {
        if (!attrName.isEmpty()) names.emplace_back(graphId + graphVs + "_" + attrName);
    }
    return names;
}

std::vector<Value> GraphInputs::exportAttrValues() const
{
    std::vector<Value> values = m_generalAttrs->values();
    values.insert(values.end(), m_graphAttrs->values().begin(), m_graphAttrs->values().end());
    return values;
}

GraphInputsPtr GraphInputs::parse(const MainApp* mainApp, const QStringList& header,
    const QStringList& values, QString& errMsg)
{
    if (header.isEmpty() || values.isEmpty()) {
        errMsg += "The 'header' and 'values' cannot be empty.";
        return nullptr;
    }

    if (header.size() != values.size()) {
        errMsg += "The 'header' and 'values' must have the same number of elements.";
        return nullptr;
    }

    auto graph = dynamic_cast<GraphPlugin*>(findPlugin(PluginType::Graph, mainApp, header, values, errMsg));
    if (!graph) {
        return nullptr; // cannot do anything without a graph
    }

    auto gi = GraphInputsPtr(new GraphInputs(graph,
        new Attributes(mainApp->generalAttrsScope().size()),
        new Attributes(graph->pluginAttrsScope().size()),
        std::vector<Cache*>()));

    // Those attributes aren't needed for the creation of a graph
    QStringList ignoredAttrs = { GENERAL_ATTR_MODELID, GENERAL_ATTR_MODELVS, GENERAL_ATTR_TRIALS, GENERAL_ATTR_AUTODELETE,
        GENERAL_ATTR_STOPAT, GENERAL_ATTR_SEED, OUTPUT_DIR, OUTPUT_HEADER };

    QStringList failedAttrs;
    parseAttrs(gi.get(), mainApp, header, values, failedAttrs);

    // make sure all attributes exist
    auto checkAll = [&failedAttrs](Attributes* attrs, const AttributesScope& attrsScope, const QStringList& ignoredAttrs) {
        for (auto const& attrRange : attrsScope) {
            if (!attrs->contains(attrRange->attrName()) && !(ignoredAttrs.contains(attrRange->attrName()))) {
                attrs->replace(attrRange->id(), attrRange->attrName(), Value());
                failedAttrs.append(attrRange->attrName());
            }
        }
    };
    checkAll(gi->m_generalAttrs, mainApp->generalAttrsScope(), ignoredAttrs);
    checkAll(gi->m_graphAttrs, graph->pluginAttrsScope(), ignoredAttrs);

    auto fixVs = [&gi](const QString& attr, quint16 vs) mutable {
        gi->m_generalAttrs->replace(gi->m_generalAttrs->indexOf(attr), attr, vs);
    };
    fixVs(GENERAL_ATTR_GRAPHVS, gi->graphPlugin()->version());

    if (!failedAttrs.isEmpty()) {
        failedAttrs.removeDuplicates();
        errMsg += QString("The following attributes are missing or invalid: %1").arg(failedAttrs.join(","));
    }

    return gi; // return the object, even if it has invalid/missing attrs
}

Plugin* GraphInputs::findPlugin(PluginType type, const MainApp* mainApp,
    const QStringList& header, const QStringList& values, QString& errMsg)
{
    QString GENERAL_ATTR_PLUGINID, GENERAL_ATTR_PLUGINVS;
    if (type == PluginType::Graph) {
        GENERAL_ATTR_PLUGINID = GENERAL_ATTR_GRAPHID;
        GENERAL_ATTR_PLUGINVS = GENERAL_ATTR_GRAPHVS;
    }
    else {
        return nullptr;
    }

    if (!header.contains(GENERAL_ATTR_PLUGINID)) {
        errMsg += " missing:" + GENERAL_ATTR_PLUGINID;
        return nullptr;
    }
    const QString pluginId = values.at(header.indexOf(GENERAL_ATTR_PLUGINID));

    // get all versions available for this plugin
    QList<quint16> availableVs = mainApp->graphs().values(pluginId);

    if (availableVs.empty()) {
        errMsg += QString(" The plugin '%1' is not available."
            " Make sure you load it before trying to add this graph.")
            .arg(pluginId);
        return nullptr;
    }

    bool vsIsInt = false;
    const int hPluginVs = header.indexOf(GENERAL_ATTR_PLUGINVS);
    quint16 validVs = hPluginVs < 0 ? 0 : values.at(hPluginVs).toUShort(&vsIsInt);
    if (!vsIsInt || !availableVs.contains(validVs)) {
        // select the newest version
        validVs = *std::max_element(availableVs.cbegin(), availableVs.cend());
        errMsg += QString(" The '%1' is missing/invalid, trying to load it with '%2 - v%3'.")
            .arg(GENERAL_ATTR_PLUGINVS, pluginId).arg(validVs);
    }

    Plugin* plugin = mainApp->plugins().value({ pluginId, validVs }, nullptr);
    Q_ASSERT(plugin);

    return plugin;
}

void GraphInputs::parseAttrs(GraphInputs* gi, const MainApp* mainApp, const QStringList& header,
    const QStringList& values, QStringList& failedAttrs)
{
    // we assume that all graph attributes start with 'uid_'
    const QString& graphId_ = gi->graphPlugin()->id() + "_";

    // get the value of each attribute and make sure they are valid
    for (int i = 0; i < values.size(); ++i) {
        const QString& vStr = values.at(i);
        QString attrName = header.at(i);

        AttributesScope::const_iterator gps = mainApp->generalAttrsScope().find(attrName);
        if (gps != mainApp->generalAttrsScope().end()) {
            Value value = gps.value()->validate(vStr);
            if (value.isValid()) {
                gi->m_generalAttrs->replace(gps.value()->id(), attrName, value);
            }
            else {
                failedAttrs.append(attrName);
            }
        }
        else {
            AttributeRangePtr attrRange;
            Attributes* pluginAttrs = nullptr;
            if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                attrRange = gi->graphPlugin()->pluginAttrRange(attrName);
                pluginAttrs = gi->m_graphAttrs;
            }

            if (pluginAttrs) {
                Value value = attrRange ? attrRange->validate(vStr) : Value();
                if (value.isValid()) {
                    pluginAttrs->replace(attrRange->id(), attrName, value);
                }
                else {
                    failedAttrs.append(attrName);
                }
            }
        }
    }
}

} // evoplex
