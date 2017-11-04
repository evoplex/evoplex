/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <vector>

#include "abstractmodel.h"
#include "attributes.h"
#include "stats.h"

namespace evoplex {

class Output
{
public:
    virtual std::vector<Value> doOperation() = 0;
};


class CustomOutput: public Output
{
public:
    CustomOutput(const AbstractModel* model, std::vector<std::string> header)
        : m_model(model), m_header(header) {}

    virtual std::vector<Value> doOperation()
    {
        return m_model->customOutputs(m_header);
    }

private:
    const AbstractModel* m_model;
    const std::vector<std::string> m_header;
};


template<typename Entity>
class DefaultOutput : public Output
{
public:
    enum Function {
        Count
    };

    DefaultOutput(Function f, int attrIdx)
        : m_func(f), m_attrIdx(attrIdx) {}

    virtual std::vector<Value> doOperation()
    {
        switch (m_func) {
        case Count:
            return Stats::count(m_entity, m_attrIdx, m_header);
            break;
        default:
            qFatal("doOperation() invalid function!");
            break;
        }
    }

private:
    const Function m_func;
    const int m_attrIdx;
    std::vector<Value> m_header;
    Entity m_entity;
};

}
#endif // UTILS_H
