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

#include <QDebug>
#include <QStringList>

#include "output.h"
#include "trial.h"
#include "utils.h"

namespace evoplex
{

Cache::Cache(const Values& inputs, const std::vector<int>& trialIds, OutputPtr parent)
    : m_parent(parent)
    , m_inputs(inputs)
{
    Q_ASSERT_X(!m_inputs.empty(), "Cache", "inputs cannot be empty");
    for (int trialId : trialIds) {
        m_trials.insert({trialId, Data()});
    }
}

void Cache::deleteCache()
{
    m_parent->deleteCache(this);
}

bool Cache::isEmpty(const int trialId) const
{
    std::unordered_map<int, Data>::const_iterator trial = m_trials.find(trialId);
    if (trial != m_trials.end()) {
        return trial->second.rows.empty();
    }
    return false;
}

QString Cache::printableHeader(const char sep, const bool joinInputs) const
{
    return Output::printableHeader(m_parent->printableHeaderPrefix(),
                                   m_inputs, sep, joinInputs);
}

void Cache::flushAll()
{
    for (auto& it : m_trials) {
        it.second.rows.clear();
    }
}

/*******************************************************/
/*******************************************************/

DefaultOutput::DefaultOutput(const Function f, const Entity e, const AttributeRange* attrRange)
    : Output()
    , m_func(f)
    , m_entity(e)
    , m_attrRange(attrRange)
{
    m_headerPrefix = QString("%1_%2_%3_").arg(
                        stringFromFunc(m_func),
                        m_entity == E_Nodes ? "nodes" : "edges",
                        m_attrRange->attrName());
}

void DefaultOutput::doOperation(const Trial* trial)
{
    if (m_allTrialIds.find(trial->id()) == m_allTrialIds.end()) {
        return;
    }

    Values allValues;
    switch (m_func) {
    case F_Count:
        if (m_entity == E_Nodes) {
            allValues = Stats::count(trial->graph()->nodes(), m_attrRange->id(), m_allInputs);
        } else {
            allValues = Stats::count(trial->graph()->edges(), m_attrRange->id(), m_allInputs);
        }
        break;
    default:
        qFatal("invalid function!");
    }
    updateCaches(trial->id(), trial->step(), allValues);
}

bool DefaultOutput::operator==(const OutputPtr output) const
{
    auto other = std::dynamic_pointer_cast<const DefaultOutput>(output);
    if (!other) return false;
    if (m_func != other->function()) return false;
    if (m_entity != other->entity()) return false;
    if (m_attrRange->id() != other->attrRange()->id()) return false;
    return true;
}

/*******************************************************/
/*******************************************************/

CustomOutput::CustomOutput() : Output()
{
    m_headerPrefix = "custom_";
}

void CustomOutput::doOperation(const Trial* trial)
{
    if (m_allTrialIds.find(trial->id()) == m_allTrialIds.end()) {
        return;
    }
    updateCaches(trial->id(), trial->step(), trial->model()->customOutputs(m_allInputs));
}

bool CustomOutput::operator==(const OutputPtr output) const
{
    auto other = std::dynamic_pointer_cast<const CustomOutput>(output);
    if (!other) return false;
    if (m_allInputs.size() != other->allInputs().size()) return false;
    for (size_t i = 0; i < m_allInputs.size(); ++i) {
        if (m_allInputs.at(i) != other->allInputs().at(i))
            return false;
    }
    return true;
}

/*******************************************************/
/*******************************************************/

Output::~Output()
{
    for (Cache* c :  m_caches) {
        delete c;
    }
}

void Output::flushAll()
{
    for (Cache* c : m_caches) {
        c->flushAll();
    }
}

Cache* Output::addCache(const Values& inputs, const std::vector<int>& trialIds)
{
    Cache* cache = new Cache(inputs, trialIds, shared_from_this());
    for (int trialId : trialIds) {
        m_allTrialIds.insert(trialId);
    }
    m_caches.emplace_back(cache);
    updateListOfInputs();
    return cache;
}

void Output::deleteCache(Cache* cache)
{
    std::vector<Cache*>::iterator it = std::find(m_caches.begin(), m_caches.end(), cache);
    if (it == m_caches.end()) {
        qFatal("tried to remove a non-existent cache.");
    }
    m_caches.erase(it);
    updateListOfInputs();
    delete cache;
    cache = nullptr;
}

void Output::updateListOfInputs()
{
    m_allTrialIds.clear();
    m_allInputs.clear();
    for (Cache* cache : m_caches) {
        m_allInputs.insert(m_allInputs.end(), cache->m_inputs.cbegin(), cache->m_inputs.cend());
        for (const auto& it : cache->m_trials) {
            m_allTrialIds.insert(it.first);
        }
    }
    // remove duplicates
    std::sort(m_allInputs.begin(), m_allInputs.end());
    m_allInputs.erase(std::unique(m_allInputs.begin(), m_allInputs.end()), m_allInputs.end());
}

void Output::updateCaches(const int trialId, const int currStep, const Values& allValues)
{
    if (m_caches.size() == 0) {
        return;
    }

    for (Cache* cache : m_caches) {
        std::unordered_map<int, Cache::Data>::iterator itData = cache->m_trials.find(trialId);
        if (itData == cache->m_trials.end()) {
            continue;
        }

        Cache::Data& data = itData->second;
        Cache::Row newRow;
        newRow.first = currStep;
        newRow.second.reserve(cache->m_inputs.size());

        for (const Value& input : cache->m_inputs) {
            const size_t col = std::find(m_allInputs.begin(), m_allInputs.end(), input) - m_allInputs.begin();
            newRow.second.emplace_back(allValues.at(col));
        }
        if (data.rows.empty()) data.last = data.rows.before_begin();
        data.last = data.rows.emplace_after(data.last, newRow);
    }
}

QString Output::printableHeader(const char sep, const bool joinInputs) const
{
    return printableHeader(m_headerPrefix, m_allInputs, sep, joinInputs);
}

QString Output::printableHeader(const QString& prefix, const Values& inputs,
                                const char sep, const bool joinInputs)
{
    QString ret;
    if (joinInputs) {
        ret = prefix;
        for (const Value& val : inputs) {
            ret += val.toQString() + sep;
        }
    } else {
        for (const Value& val : inputs) {
            ret += prefix + val.toQString() + sep;
        }
    }
    ret.chop(1);
    return ret;
}

std::vector<Cache*> Output::parseHeader(const QStringList& header, const std::vector<int>& trialIds,
                                        const ModelPlugin* model, QString& errorMsg)
{
    std::vector<Cache*> caches;
    std::vector<Value> customHeader;
    for (QString h : header) {
        if (h.startsWith("custom_")) {
            h.remove("custom_");
            if (h.isEmpty()) {
                errorMsg = "invalid header! Custom function cannot be empty.\n";
                qWarning() << errorMsg;
                Utils::deleteAndShrink(caches);
                return caches;
            }
            customHeader.emplace_back(h);
            continue;
        }

        DefaultOutput::Function func;
        if (h.startsWith("count_")) {
            h.remove("count_");
            func = DefaultOutput::F_Count;
        } else {
            errorMsg = QString("invalid header! Function does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(caches);
            return caches;
        }

        AttributesScope entityAttrsScope;
        DefaultOutput::Entity entity;
        if (h.startsWith("nodes_")) {
            h.remove("nodes_");
            entity = DefaultOutput::E_Nodes;
            entityAttrsScope = model->nodeAttrsScope();
        } else if (h.startsWith("edges_")) {
            h.remove("edges_");
            entity = DefaultOutput::E_Edges;
            entityAttrsScope = model->edgeAttrsScope();
        } else {
            errorMsg = QString("invalid header! Entity does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(caches);
            return caches;
        }

        QStringList attrHeaderStr = h.split("_");
        AttributeRange* attrRange = entityAttrsScope.value(attrHeaderStr.first());
        if (!attrRange->isValid()) {
            errorMsg = QString("invalid header! Attribute does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(caches);
            return caches;
        }

        std::vector<Value> attrHeader; //inputs
        attrHeaderStr.removeFirst();
        for (const QString& valStr : attrHeaderStr) {
            Value val = attrRange->validate(valStr);
            if (!val.isValid()) {
                errorMsg = QString("invalid header! Value of attribute is invalid. (%1)\n").arg(valStr);
                qWarning() << errorMsg;
                Utils::deleteAndShrink(caches);
                return caches;
            }
            attrHeader.emplace_back(val);
        }

        if (attrHeader.empty()) {
            errorMsg = "invalid header! Function cannot be empty.\n";
            qWarning() << errorMsg;
            Utils::deleteAndShrink(caches);
            return caches;
        }

        OutputPtr output = std::make_shared<DefaultOutput>(func, entity, attrRange);
        caches.emplace_back(output->addCache(attrHeader, trialIds));
    }

    if (!customHeader.empty()) {
        OutputPtr output = std::make_shared<CustomOutput>();
        caches.emplace_back(output->addCache(customHeader, trialIds));
    }

    return caches;
}
}
