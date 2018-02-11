/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QStringList>

#include "output.h"
#include "utils.h"

namespace evoplex {

DefaultOutput::DefaultOutput(const Function f, const Entity e, const ValueSpace *valSpace,
                             Values inputs, const std::vector<int> trialIds)
    : Output(inputs, trialIds)
    , m_func(f)
    , m_entity(e)
    , m_valueSpace(valSpace)
{
}

void DefaultOutput::doOperation(const int trialId, const AbstractModel* model)
{
    if (m_trialIds.find(trialId) == m_trialIds.end()) {
        return;
    }

    Values allValues;
    switch (m_func) {
    case F_Count:
        if (m_entity == E_Agents) {
            allValues = Stats::count(model->graph()->agents(), m_valueSpace->id(), m_allInputs);
        } else {
            allValues = Stats::count(model->graph()->edges(), m_valueSpace->id(), m_allInputs);
        }
        break;
    default:
        qFatal("doOperation() invalid function!");
        break;
    }
    updateCaches(trialId, model->currStep(), allValues);
}

QString DefaultOutput::printableHeader(const char sep) const
{
    QString prefix = QString("%1_%2_%3_")
            .arg(stringFromFunc(m_func))
            .arg((m_entity == E_Agents ? "agents" : "edges"))
            .arg(m_valueSpace->attrName());

    QString ret;
    for (Value val : m_allInputs) {
        ret += prefix + val.toQString() + sep;
    }
    ret.chop(1);
    return ret;
}

bool DefaultOutput::operator==(const Output* output) const
{
    const DefaultOutput* other = dynamic_cast<const DefaultOutput*>(output);
    if (!other) return false;
    if (m_func != other->function()) return false;
    if (m_entity != other->entity()) return false;
    if (m_valueSpace->id() != other->valueSpace()->id()) return false;
    return true;
}

/*******************************************************/
/*******************************************************/

CustomOutput::CustomOutput(Values inputs, const std::vector<int> trialIds)
        : Output(inputs, trialIds)
{
}

void CustomOutput::doOperation(const int trialId, const AbstractModel* model)
{
    if (m_trialIds.find(trialId) == m_trialIds.end()) {
        return;
    }
    updateCaches(trialId, model->currStep(), model->customOutputs(m_allInputs));
}

QString CustomOutput::printableHeader(const char sep) const
{
    QString ret;
    for (Value h : m_allInputs) {
        ret += "custom_" + h.toQString() + sep;
    }
    ret.chop(1);
    return ret;
}

bool CustomOutput::operator==(const Output* output) const
{
    const CustomOutput* other = dynamic_cast<const CustomOutput*>(output);
    if (!other) return false;
    if (m_allInputs.size() != other->allInputs().size()) return false;
    for (int i = 0; i < m_allInputs.size(); ++i) {
        if (m_allInputs.at(i) != other->allInputs().at(i))
            return false;
    }
    return true;
}

/*******************************************************/
/*******************************************************/

Output::Output(Values inputs, const std::vector<int> trialIds)
    : m_lastCacheId(-1)
{
    addCache(inputs, trialIds);
}

bool Output::isEmpty(const int cacheId, const int trialId) const
{
    try {
        return  m_caches.at(cacheId).trials.at(trialId).rows.empty();
    } catch (...) {
        return false;
    }
}

const int Output::addCache(Values inputs, const std::vector<int> trialIds)
{
    Cache cache;
    cache.inputs = inputs;
    for (int trialId : trialIds) {
        cache.trials.insert({trialId, Data()});
        m_trialIds.insert(trialId);
    }
    m_caches.insert({++m_lastCacheId, cache});
    updateListOfInputs();
    return m_lastCacheId;
}

void Output::removeCache(const int cacheId, const int trialId)
{
    m_trialIds.erase(trialId);
    m_caches.at(cacheId).trials.erase(trialId);
    if (m_caches.at(cacheId).trials.empty()) {
        m_caches.erase(cacheId);
        updateListOfInputs();
    }
}

void Output::updateListOfInputs()
{
    m_allInputs.clear();
    for (auto& itCache : m_caches) {
        const Values& inputs = itCache.second.inputs;
        m_allInputs.insert(m_allInputs.end(), inputs.begin(), inputs.end());
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

    for (auto& itCache : m_caches) {
        Cache& cache = itCache.second;
        std::unordered_map<int,Data>::iterator itData = cache.trials.find(trialId);
        if (itData == cache.trials.end()) {
            continue;
        }

        Data& data = itData->second;
        Row newRow;
        newRow.first = currStep;
        newRow.second.reserve(cache.inputs.size());

        for (Value input : cache.inputs) {
            int col = std::find(m_allInputs.begin(), m_allInputs.end(), input) - m_allInputs.begin();
            Q_ASSERT(col != m_allInputs.size()); // input must exist
            newRow.second.emplace_back(allValues.at(col));
        }
        if (data.rows.empty()) data.last = data.rows.before_begin();
        data.last = data.rows.emplace_after(data.last, newRow);
    }
}

std::vector<Output*> Output::parseHeader(const QStringList& header, const std::vector<int> trialIds,
                                         const ModelPlugin* model, QString& errorMsg)
{
    std::vector<Output*> outputs;
    std::vector<Value> customHeader;
    for (QString h : header) {
        if (h.startsWith("custom_")) {
            h.remove("custom_");
            if (h.isEmpty()) {
                errorMsg = "[OutputHeader] invalid header! Custom function cannot be empty.\n";
                qWarning() << errorMsg;
                Utils::deleteAndShrink(outputs);
                return outputs;
            }
            customHeader.emplace_back(h);
            continue;
        }

        DefaultOutput::Function func;
        if (h.startsWith("count_")) {
            h.remove("count_");
            func = DefaultOutput::F_Count;
        } else {
            errorMsg = QString("[OutputHeader] invalid header! Function does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(outputs);
            return outputs;
        }

        AttributesSpace entityAttrSpace;
        DefaultOutput::Entity entity;
        if (h.startsWith("agents_")) {
            h.remove("agents_");
            entity = DefaultOutput::E_Agents;
            entityAttrSpace = model->agentAttrSpace();
        } else if (h.startsWith("edges_")) {
            h.remove("edges_");
            entity = DefaultOutput::E_Edges;
            entityAttrSpace = model->agentAttrSpace();
        } else {
            errorMsg = QString("[OutputHeader] invalid header! Entity does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(outputs);
            return outputs;
        }

        QStringList attrHeaderStr = h.split("_");
        ValueSpace* valSpace = entityAttrSpace.value(attrHeaderStr.first());
        if (!valSpace->isValid()) {
            errorMsg = QString("[OutputHeader] invalid header! Attribute does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            Utils::deleteAndShrink(outputs);
            return outputs;
        }

        std::vector<Value> attrHeader; //inputs
        attrHeaderStr.removeFirst();
        for (QString valStr : attrHeaderStr) {
            Value val = valSpace->validate(valStr);
            if (!val.isValid()) {
                errorMsg = QString("[OutputHeader] invalid header! Value of attribute is invalid. (%1)\n").arg(valStr);
                qWarning() << errorMsg;
                Utils::deleteAndShrink(outputs);
                return outputs;
            }
            attrHeader.emplace_back(val);
        }

        if (attrHeader.empty()) {
            errorMsg = "[OutputHeader] invalid header! Function cannot be empty.\n";
            qWarning() << errorMsg;
            Utils::deleteAndShrink(outputs);
            return outputs;
        }

        outputs.emplace_back(new DefaultOutput(func, entity, valSpace, attrHeader, trialIds));
    }

    if (!customHeader.empty()) {
        outputs.emplace_back(new CustomOutput(customHeader, trialIds));
    }

    return outputs;
}
}
