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
    virtual std::vector<Value> doOperation(const AbstractModel* model) = 0;

    // Printable header with all columns of this operation separated by commas.
    // Format:
    // - CustomOutput : "custom_nameDefinedInTheModel"
    // - DefaultOutput : "function_entity_attrName_value"
    virtual QString printableHeader() = 0;

    static std::vector<Output*> parseHeader(const QStringList& header, const Attributes& agentAttrMin,
                                            const Attributes &edgeAttrMin, QString &errorMsg);
};


class CustomOutput: public Output
{
public:
    CustomOutput(const std::vector<std::string>& header);

    virtual std::vector<Value> doOperation(const AbstractModel* model);
    virtual QString printableHeader();

private:
    const std::vector<std::string> m_header;
};


class DefaultOutput : public Output
{
public:
    enum Function {
        F_Invalid,
        F_Count
    };

    enum Entity {
        E_Agents,
        E_Edges
    };

    DefaultOutput(Function f, Entity e, const QString& attrName,
                  int attrIdx, const std::vector<Value>& header);

    virtual std::vector<Value> doOperation(const AbstractModel* model);
    virtual QString printableHeader();

private:
    const Function m_func;
    const Entity m_entity;
    const QString m_attrName;
    const int m_attrIdx;
    const std::vector<Value> m_header;
};

}
#endif // UTILS_H
