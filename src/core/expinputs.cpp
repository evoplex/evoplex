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

ExpInputs::ExpInputs(Attributes* general, Attributes* graph,
                     Attributes* model, std::vector<Cache*> caches)
    : m_generalAttrs(general),
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

std::vector<QString> ExpInputs::exportAttrNames() const
{
    std::vector<QString> names = m_generalAttrs->names();
    const QString graphId = m_generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHID, "").toQString();
    const QString modelId = m_generalAttrs->value(GENERAL_ATTRIBUTE_MODELID, "").toQString();
    for (const QString& attrName : m_graphAttrs->names()) {
        if (!attrName.isEmpty()) names.emplace_back(graphId + "_" + attrName);
    }
    for (const QString& attrName : m_modelAttrs->names()) {
        if (!attrName.isEmpty()) names.emplace_back(modelId + "_" + attrName);
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

ExpInputs* ExpInputs::parse(const MainApp* mainApp, const QStringList& header,
                            const QStringList& values, QString& errMsg)
{
    if (header.isEmpty() || values.isEmpty()) {
        errMsg += "The 'header' and 'values' cannot be empty.";
        return nullptr;
    } else if (header.size() != values.size()) {
        errMsg += "The 'header' and 'values' must have the same number of elements.";
        return nullptr;
    }

    Plugins plugins = findPlugins(mainApp, header, values, errMsg);
    if (!errMsg.isEmpty()) {
        return nullptr;
    }

    ExpInputs* ei = new ExpInputs(new Attributes(mainApp->generalAttrsScope().size()),
                                  new Attributes(plugins.first->pluginAttrsScope().size()),
                                  new Attributes(plugins.second->pluginAttrsScope().size()),
                                  std::vector<Cache*>());

    QStringList failedAttrs;
    parseAttrs(mainApp, plugins, header, values, ei, failedAttrs);
    parseFileCache(plugins.second, ei, failedAttrs, errMsg);

    // make sure all attributes exist
    auto checkAll = [&failedAttrs](const Attributes* attrs, const AttributesScope& attrsScope) {
        for (const AttributeRange* attrRange : attrsScope) {
            if (!attrs->contains(attrRange->attrName())) {
                failedAttrs.append(attrRange->attrName());
            }
        }
    };
    checkAll(ei->m_generalAttrs, mainApp->generalAttrsScope());
    checkAll(ei->m_graphAttrs, plugins.first->pluginAttrsScope());
    checkAll(ei->m_modelAttrs, plugins.second->pluginAttrsScope());

    if (!failedAttrs.isEmpty()) {
        failedAttrs.removeDuplicates();
        errMsg += QString("The following attributes are missing/invalid: %1").arg(failedAttrs.join(","));
    }

    return ei; // return the object, even if it has invalid/missing attrs
}

ExpInputs::Plugins ExpInputs::findPlugins(const MainApp* mainApp,
        const QStringList& header, const QStringList& values, QString& errMsg)
{
    // find the model and graph for this experiment
    const int headerGraphId = header.indexOf(GENERAL_ATTRIBUTE_GRAPHID);
    const int headerModelId = header.indexOf(GENERAL_ATTRIBUTE_MODELID);
    if (headerGraphId < 0 || headerModelId < 0) {
        errMsg += "The experiment should have both graphId and modelId.";
        return Plugins();
    }

    // check if the model and graph are available
    Plugins plugins = std::make_pair(mainApp->graph(values.at(headerGraphId)),
                                     mainApp->model(values.at(headerModelId)));
    if (!plugins.first) {
        errMsg += QString("The graph plugin '%1' is not available."
                          " Make sure you load it before trying to add this experiment.")
                          .arg(values.at(headerGraphId));
        return Plugins();
    }
    if (!plugins.second) {
        errMsg += QString("The model plugin '%1' is not available."
                          " Make sure you load it before trying to add this experiment.")
                          .arg(values.at(headerModelId));
        return Plugins();
    }

    // make sure that the chosen graphId is allowed in this model
    if (!plugins.second->graphIsSupported(plugins.first->id())) {
        QString supportedGraphs = plugins.second->supportedGraphs().toList().join(", ");
        errMsg = QString("The graph plugin '%1' cannot be used in this model (%2). The allowed ones are: %3")
                         .arg(plugins.first->id()).arg(plugins.second->id()).arg(supportedGraphs);
        return Plugins();
    }
    return plugins;
}

void ExpInputs::parseAttrs(const MainApp* mainApp, Plugins plugins,
                           const QStringList& header, const QStringList& values,
                           ExpInputs* ei, QStringList& failedAttrs)
{
    // we assume that all graph/model attributes start with 'uid_'
    const QString& graphId_ = plugins.first->id() + "_";
    const QString& modelId_ = plugins.second->id() + "_";

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
            const AttributeRange* attrRange = nullptr;
            Attributes* pluginAttrs = nullptr;
            if (attrName.startsWith(modelId_)) {
                attrName = attrName.remove(modelId_);
                attrRange = plugins.second->pluginAttrRange(attrName);
                pluginAttrs = ei->m_modelAttrs;
            } else if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                attrRange = plugins.first->pluginAttrRange(attrName);
                pluginAttrs = ei->m_graphAttrs;
            }

            if (pluginAttrs) {
                Value value;
                if (attrRange) {
                    value = attrRange->validate(vStr);
                }

                if (value.isValid()) {
                    pluginAttrs->replace(attrRange->id(), attrName, value);
                } else {
                    failedAttrs.append(attrName);
                }
            }
        }
    }
}

void ExpInputs::parseFileCache(const ModelPlugin* mPlugin, ExpInputs* ei,
                               QStringList& failedAttrs, QString& errMsg)
{
    QString outHeader = ei->m_generalAttrs->value(OUTPUT_HEADER, Value("")).toQString();
    if (failedAttrs.isEmpty() && !outHeader.isEmpty()) {
        const int numTrials = ei->m_generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt();
        Q_ASSERT_X(numTrials > 0, "ExpInputs", "what? an experiment without trials?");
        std::vector<int> trialIds;
        for (int i = 0; i < numTrials; ++i) {
            trialIds.emplace_back(i);
        }

        QStringList _outHeader = outHeader.split(";", QString::SkipEmptyParts);
        ei->m_fileCaches = Output::parseHeader(_outHeader, trialIds, mPlugin, errMsg);
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
