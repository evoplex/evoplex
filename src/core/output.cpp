/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QStringList>

#include "output.h"
#include "utils.h"

namespace evoplex {

DefaultOutput::DefaultOutput(Function f, Entity e, const QString& attrName, int attrIdx, Values inputs)
    : Output(inputs)
    , m_func(f)
    , m_entity(e)
    , m_attrName(attrName)
    , m_attrIdx(attrIdx)
{
}

void DefaultOutput::doOperation(const AbstractModel* model)
{
    Values allValues;
    switch (m_func) {
    case F_Count:
        if (m_entity == E_Agents)
            allValues = Stats::count(model->graph()->agents(), m_attrIdx, m_inputs);
        else
            allValues = Stats::count(model->graph()->edges(), m_attrIdx, m_inputs);
        break;
    default:
        qFatal("doOperation() invalid function!");
        break;
    }
    updateCaches(allValues);
}

QString DefaultOutput::printableHeader()
{
    QString prefix = QString("%1_%2_%3_")
            .arg(stringFromFunc(m_func))
            .arg((m_entity == E_Agents ? "agents" : "edges"))
            .arg(m_attrName);

    QString ret;
    for (Value val : m_inputs) {
        ret += prefix + val.toQString() + ",";
    }
    ret.chop(1);
    return ret;
}

bool DefaultOutput::operator==(const Output* output)
{
    const DefaultOutput* other = dynamic_cast<const DefaultOutput*>(output);
    if (!other) return false;
    if (m_func != other->function()) return false;
    if (m_entity != other->entity()) return false;
    if (m_attrIdx != other->attrIdx()) return false;
    return true;
}

/*******************************************************/
/*******************************************************/

CustomOutput::CustomOutput(Values inputs)
        : Output(inputs)
{
}

void CustomOutput::doOperation(const AbstractModel* model)
{
    updateCaches(model->customOutputs(m_inputs));
}

QString CustomOutput::printableHeader()
{
    QString ret;
    for (Value h : m_inputs) {
        ret += h.toQString() + ",";
    }
    ret.chop(1);
    return ret;
}

bool CustomOutput::operator==(const Output* output)
{
    const CustomOutput* other = dynamic_cast<const CustomOutput*>(output);
    if (!other) return false;
    if (m_inputs.size() != other->inputs().size()) return false;
    for (int i = 0; i < m_inputs.size(); ++i) {
        if (m_inputs.at(i) != other->inputs().at(i))
            return false;
    }
    return true;
}

/*******************************************************/
/*******************************************************/

Output::Output(Values inputs)
{
    addCache(inputs);
}

const int Output::addCache(Values inputs)
{
    m_inputs.insert(m_inputs.end(), inputs.begin(), inputs.end());
    Cache c;
    c.inputs = inputs;
    m_caches.emplace_back(c);
    return m_caches.size() - 1;
}

void Output::updateCaches(const Values& allValues)
{
    if (m_caches.size() == 0) {
        return;
    }

    // the first cache will always hold the initial input
    Cache& cache = m_caches.at(0);
    Values vals;
    vals.insert(vals.end(), allValues.begin(), allValues.begin() + cache.inputs.size());
    if (cache.rows.empty()) cache.last = cache.rows.before_begin();
    cache.last = cache.rows.emplace_after(cache.last, vals);

    for (int cacheId = 1; cacheId < m_caches.size(); ++cacheId) {
        Cache& cache = m_caches.at(cacheId);
        Values vals;
        vals.reserve(cache.inputs.size());
        for (Value input : cache.inputs) {
            int col = std::find(m_inputs.begin(), m_inputs.end(), input) - m_inputs.begin();
            Q_ASSERT(col != m_inputs.size()); // input must exist
            vals.emplace_back(allValues.at(col));
        }
        if (cache.rows.empty()) cache.last = cache.rows.before_begin();
        cache.last = cache.rows.emplace_after(cache.last, vals);
    }
}

std::vector<Output*> Output::parseHeader(const QStringList& header, const Attributes& agentAttrMin,
                                         const Attributes& edgeAttrMin, QString& errorMsg)
{
    std::vector<Output*> outputs;
    std::vector<Value> customHeader;
    for (QString h : header) {
        if (h.startsWith("custom_")) {
            h.remove("custom_");
            if (h.isEmpty()) {
                errorMsg = "[OutputHeader] invalid header! Custom function cannot be empty.\n";
                qWarning() << errorMsg;
                qDeleteAll(outputs);
                outputs.clear();
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
            qDeleteAll(outputs);
            outputs.clear();
            return outputs;
        }

        Attributes entityAttrMin;
        DefaultOutput::Entity entity;
        if (h.startsWith("agents_")) {
            h.remove("agents_");
            entity = DefaultOutput::E_Agents;
            entityAttrMin = agentAttrMin;
        } else if (h.startsWith("edges_")) {
            h.remove("edges_");
            entity = DefaultOutput::E_Edges;
            entityAttrMin = edgeAttrMin;
        } else {
            errorMsg = QString("[OutputHeader] invalid header! Entity does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            qDeleteAll(outputs);
            outputs.clear();
            return outputs;
        }

        QStringList attrHeaderStr = h.split("_");
        int attrIdx = entityAttrMin.indexOf(attrHeaderStr.first());
        if (attrIdx < 0) {
            errorMsg = QString("[OutputHeader] invalid header! Attribute does not exist. (%1)\n").arg(h);
            qWarning() << errorMsg;
            qDeleteAll(outputs);
            outputs.clear();
            return outputs;
        }

        std::vector<Value> attrHeader; //inputs
        attrHeaderStr.removeFirst();
        for (QString valStr : attrHeaderStr) {
            Value val = Utils::valueFromString(entityAttrMin.value(attrIdx).type, valStr);
            if (!val.isValid()) {
                errorMsg = QString("[OutputHeader] invalid header! Value of attribute is invalid. (%1)\n").arg(valStr);
                qWarning() << errorMsg;
                qDeleteAll(outputs);
                outputs.clear();
                return outputs;
            }
            attrHeader.emplace_back(val);
        }

        if (attrHeader.empty()) {
            errorMsg = "[OutputHeader] invalid header! Function cannot be empty.\n";
            qWarning() << errorMsg;
            qDeleteAll(outputs);
            outputs.clear();
            return outputs;
        }

        outputs.emplace_back(new DefaultOutput(func, entity, entityAttrMin.name(attrIdx), attrIdx, attrHeader));
    }

    if (!customHeader.empty()) {
        outputs.emplace_back(new CustomOutput(customHeader));
    }

    return outputs;
}
}
