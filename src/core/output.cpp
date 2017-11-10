/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QStringList>

#include "output.h"
#include "utils.h"

namespace evoplex {

DefaultOutput::DefaultOutput(Function f, Entity e, const QString& attrName, int attrIdx,
                             const Values& inputs, bool enableCache)
    : Output(enableCache)
    , m_func(f)
    , m_entity(e)
    , m_attrName(attrName)
    , m_attrIdx(attrIdx)
    , m_inputs(inputs)
{
}

Values DefaultOutput::doOperation(const AbstractModel* model)
{
    Values vals;

    switch (m_func) {
    case F_Count:
        if (m_entity == E_Agents)
            vals = Stats::count(model->graph()->agents(), m_attrIdx, m_header);
        else
            vals = Stats::count(model->graph()->edges(), m_attrIdx, m_header);
        break;
    default:
        qFatal("doOperation() invalid function!");
        break;
    }

    if (m_cacheEnabled) {
        m_cache.emplace_back(vals);
    }
    return vals;
}

QString DefaultOutput::printableHeader()
{
    QString func;
    if (m_func == F_Count) func = "count";
    else func = "invalid";

    QString prefix = QString("%1_%2_%3_")
            .arg(func)
            .arg((m_entity == E_Agents ? "agents" : "edges"))
            .arg(m_attrName);

    QString ret;
    for (Value val : m_header) {
        ret += prefix + val.toQString() + ",";
    }
    ret.chop(1);
    return ret;
}

bool DefaultOutput::operator==(const Output* output)
{
    DefaultOutput* other = dynamic_cast<DefaultOutput*>(ouput);
    if (!other) return false;
    if (m_func != other->function()) return false;
    if (m_entity != other->entity()) return false;
    // TODO
}

/*******************************************************/
/*******************************************************/

CustomOutput::CustomOutput(const std::vector<std::string>& header, bool enableCache)
        : Output(enableCache)
        , m_header(header)
{
}

Values CustomOutput::doOperation(const AbstractModel* model)
{
    Values vals = model->customOutputs(m_header);
    if (m_cacheEnabled) {
        m_cache.emplace_back(vals);
    }
    return vals;
}

QString CustomOutput::printableHeader()
{
    QString ret;
    for (std::string h : m_header) {
        ret += QString::fromStdString(h) + ",";
    }
    ret.chop(1);
    return ret;
}

/*******************************************************/
/*******************************************************/

Output::Output(bool enableCache)
    : m_cacheEnabled(enableCache)
{
}

void Output::flushCache()
{
    m_cache.clear();
    m_cache.shrink_to_fit();
}

std::vector<Output*> Output::parseHeader(const QStringList& header, const Attributes& agentAttrMin,
                                         const Attributes& edgeAttrMin, QString& errorMsg)
{
    std::vector<Output*> outputs;
    std::vector<std::string> customHeader;
    for (QString h : header) {
        if (h.startsWith("custom_")) {
            customHeader.emplace_back(h.remove("custom_").toStdString());
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

        std::vector<Value> attrHeader;
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

        outputs.emplace_back(new DefaultOutput(func, entity, entityAttrMin.name(attrIdx), attrIdx, attrHeader, false));
    }

    if (!customHeader.empty()) {
        outputs.emplace_back(new CustomOutput(customHeader, false));
    }

    return outputs;
}
}
