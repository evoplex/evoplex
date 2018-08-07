/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#include "expinputs.h"
#include "constants.h"
#include "graphplugin.h"

namespace evoplex {

ExpInputs::ExpInputs(const GraphPlugin* g, const ModelPlugin* m, Attributes* general,
                     Attributes* graph, Attributes* model, const std::vector<Cache*>& caches)
    : m_graphPlugin(g),
      m_modelPlugin(m),
      m_generalAttrs(general),
      m_graphAttrs(graph),
      m_modelAttrs(model),
      m_fileCaches(caches)
{
}

ExpInputs::~ExpInputs()
{
    delete m_generalAttrs;
    delete m_modelAttrs;
    delete m_graphAttrs;
    for (Cache* c : m_fileCaches) {
        c->deleteCache();
    }
}

std::vector<QString> ExpInputs::exportAttrNames(bool appendVersion) const
{
    std::vector<QString> names = m_generalAttrs->names();
    const QString graphId = m_generalAttrs->value(GENERAL_ATTR_GRAPHID, "").toQString();
    const QString modelId = m_generalAttrs->value(GENERAL_ATTR_MODELID, "").toQString();
    const QString graphVs = appendVersion ? "-" + m_generalAttrs->value(GENERAL_ATTR_GRAPHVS, "").toQString() : "";
    const QString modelVs = appendVersion ? "-" + m_generalAttrs->value(GENERAL_ATTR_MODELVS, "").toQString() : "";
    for (const QString& attrName : m_graphAttrs->names()) {
        if (!attrName.isEmpty()) names.emplace_back(graphId + graphVs + "_" + attrName);
    }
    for (const QString& attrName : m_modelAttrs->names()) {
        if (!attrName.isEmpty()) names.emplace_back(modelId + modelVs + "_" + attrName);
    }
    return names;
}

std::vector<Value> ExpInputs::exportAttrValues() const
{
    std::vector<Value> values = m_generalAttrs->values();
    values.insert(values.end(), m_graphAttrs->values().begin(), m_graphAttrs->values().end());
    values.insert(values.end(), m_modelAttrs->values().begin(), m_modelAttrs->values().end());
    return values;
}

ExpInputsPtr ExpInputs::parse(const MainApp* mainApp, const QStringList& header,
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

    auto model = dynamic_cast<ModelPlugin*>(findPlugin(PluginType::Model, mainApp, header, values, errMsg));
    if (!model) {
        return nullptr; // cannot do anything without a model
    }

    auto graph = dynamic_cast<GraphPlugin*>(findPlugin(PluginType::Graph, mainApp, header, values, errMsg));
    if (!graph) {
        return nullptr; // cannot do anything without a graph
    }

    // make sure that the chosen graphId is allowed in this model
    if (!model->graphIsSupported(graph->id())) {
        QString supportedGraphs = model->supportedGraphs().toList().join(", ");
        errMsg = QString("The graph plugin '%1' cannot be used in this model (%2). "
                "The allowed ones are: %3").arg(graph->id(), model->id(), supportedGraphs);
        return nullptr;
    }

    auto ei = ExpInputsPtr(new ExpInputs(graph, model,
        new Attributes(mainApp->generalAttrsScope().size()),
        new Attributes(graph->pluginAttrsScope().size()),
        new Attributes(model->pluginAttrsScope().size()),
        std::vector<Cache*>()));

    QStringList failedAttrs;
    parseAttrs(ei.get(), mainApp, header, values, failedAttrs);
    parseFileCache(ei.get(), failedAttrs, errMsg);

    // make sure all attributes exist
    auto checkAll = [&failedAttrs](Attributes* attrs, const AttributesScope& attrsScope) {
        for (auto const& attrRange : attrsScope) {
            if (!attrs->contains(attrRange->attrName())) {
                attrs->replace(attrRange->id(), attrRange->attrName(), Value());
                failedAttrs.append(attrRange->attrName());
            }
        }
    };
    checkAll(ei->m_generalAttrs, mainApp->generalAttrsScope());
    checkAll(ei->m_graphAttrs, graph->pluginAttrsScope());
    checkAll(ei->m_modelAttrs, model->pluginAttrsScope());

    // even when something goes wrong, if the experiment has found a valid model
    // and graph, it can be opened by the user. In those cases, the modelVersion
    // and graphVersion might be empty or not the same as the loaded plugins.
    // Thus, to avoid incosistency between the plugin objects and the value of
    // the attributes in the ExpInputs, here we ensure that those fields
    // correspond to the same thing.
    auto fixVs = [&ei](const QString& attr, quint16 vs) mutable {
        ei->m_generalAttrs->replace(ei->m_generalAttrs->indexOf(attr), attr, vs);
    };
    fixVs(GENERAL_ATTR_GRAPHVS, ei->graphPlugin()->version());
    fixVs(GENERAL_ATTR_MODELVS, ei->modelPlugin()->version());

    if (!failedAttrs.isEmpty()) {
        failedAttrs.removeDuplicates();
        errMsg += QString("The following attributes are missing or invalid: %1").arg(failedAttrs.join(","));
    }

    return ei; // return the object, even if it has invalid/missing attrs
}

Plugin* ExpInputs::findPlugin(PluginType type, const MainApp* mainApp,
        const QStringList& header, const QStringList& values, QString& errMsg)
{
    QString GENERAL_ATTR_PLUGINID, GENERAL_ATTR_PLUGINVS;
    if (type == PluginType::Graph) {
        GENERAL_ATTR_PLUGINID = GENERAL_ATTR_GRAPHID;
        GENERAL_ATTR_PLUGINVS = GENERAL_ATTR_GRAPHVS;
    } else if (type == PluginType::Model) {
        GENERAL_ATTR_PLUGINID = GENERAL_ATTR_MODELID;
        GENERAL_ATTR_PLUGINVS = GENERAL_ATTR_MODELVS;
    } else {
        return nullptr;
    }

    if (!header.contains(GENERAL_ATTR_PLUGINID)) {
        errMsg += " missing:" + GENERAL_ATTR_PLUGINID;
        return nullptr;
    }
    const QString pluginId = values.at(header.indexOf(GENERAL_ATTR_PLUGINID));

    // get all versions available for this plugin
    QList<quint16> availableVs = type == PluginType::Graph
            ? mainApp->graphs().values(pluginId)
            : mainApp->models().values(pluginId);

    if (availableVs.empty()) {
        errMsg += QString(" The plugin '%1' is not available."
                " Make sure you load it before trying to add this experiment.")
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

    Plugin* plugin = mainApp->plugins().value({pluginId, validVs}, nullptr);
    Q_ASSERT(plugin);

    return plugin;
}

void ExpInputs::parseAttrs(ExpInputs* ei, const MainApp* mainApp, const QStringList& header,
                           const QStringList& values, QStringList& failedAttrs)
{
    // we assume that all graph/model attributes start with 'uid_'
    const QString& graphId_ = ei->graphPlugin()->id() + "_";
    const QString& modelId_ = ei->modelPlugin()->id() + "_";

    // get the value of each attribute and make sure they are valid
    for (int i = 0; i < values.size(); ++i) {
        const QString& vStr = values.at(i);
        QString attrName = header.at(i);

        AttributesScope::const_iterator gps = mainApp->generalAttrsScope().find(attrName);
        if (gps != mainApp->generalAttrsScope().end()) {
            Value value = gps.value()->validate(vStr);
            if (value.isValid()) {
                ei->m_generalAttrs->replace(gps.value()->id(), attrName, value);
            } else {
                failedAttrs.append(attrName);
            }
        } else {
            AttributeRangePtr attrRange;
            Attributes* pluginAttrs = nullptr;
            if (attrName.startsWith(modelId_)) {
                attrName = attrName.remove(modelId_);
                attrRange = ei->modelPlugin()->pluginAttrRange(attrName);
                pluginAttrs = ei->m_modelAttrs;
            } else if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                attrRange = ei->graphPlugin()->pluginAttrRange(attrName);
                pluginAttrs = ei->m_graphAttrs;
            }

            if (pluginAttrs) {
                Value value = attrRange ? attrRange->validate(vStr) : Value();
                if (value.isValid()) {
                    pluginAttrs->replace(attrRange->id(), attrName, value);
                } else {
                    failedAttrs.append(attrName);
                }
            }
        }
    }
}

void ExpInputs::parseFileCache(ExpInputs* ei, QStringList& failedAttrs, QString& errMsg)
{
    QString outHeader = ei->m_generalAttrs->value(OUTPUT_HEADER, Value("")).toQString();
    if (failedAttrs.isEmpty() && !outHeader.isEmpty()) {
        const int numTrials = ei->m_generalAttrs->value(GENERAL_ATTR_TRIALS).toInt();
        Q_ASSERT_X(numTrials > 0, "ExpInputs", "what? an experiment without trials?");
        std::vector<int> trialIds;
        trialIds.reserve(static_cast<size_t>(numTrials));
        for (int i = 0; i < numTrials; ++i) {
            trialIds.emplace_back(i);
        }

        QStringList _outHeader = outHeader.split(";", QString::SkipEmptyParts);
        ei->m_fileCaches = Output::parseHeader(_outHeader, trialIds, ei->modelPlugin(), errMsg);
        if (ei->m_fileCaches.empty()) {
            failedAttrs.append(OUTPUT_HEADER);
        }

        QFileInfo outDir(ei->m_generalAttrs->value(OUTPUT_DIR, Value("")).toQString());
        if (!outDir.isDir() || !outDir.isWritable()) {
            errMsg += "The output directory must be valid and writable!\n";
            failedAttrs.append(OUTPUT_DIR);
        }
    }
}



} // evoplex
