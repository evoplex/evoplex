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
    explicit Output(bool enableCache = false);

    virtual Values doOperation(const AbstractModel* model) = 0;

    // Printable header with all columns of this operation separated by commas.
    virtual QString printableHeader() = 0;

    virtual bool operator==(const Output& output) = 0;

    static std::vector<Output*> parseHeader(const QStringList& header,
            const Attributes& agentAttrMin, const Attributes &edgeAttrMin, QString &errorMsg);

    const std::vector<Values> readCache() const { return m_cache; }
    void flushCache();

    inline bool cacheEnabled() const { return m_cacheEnabled; }
    inline void setEnableCache(bool enabled) { m_cacheEnabled = enabled; }

protected:
    bool m_cacheEnabled;
    std::vector<Values> m_cache;
};


class CustomOutput: public Output
{
public:
    explicit CustomOutput(const std::vector<std::string>& header, bool enableCache);

    virtual Values doOperation(const AbstractModel* model);

    // Printable header with all columns of this operation separated by commas.
    // Format: "custom_nameDefinedInTheModel"
    virtual QString printableHeader();

    virtual bool operator==(const Output& output);

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

    explicit DefaultOutput(Function f, Entity e, const QString& attrName, int attrIdx,
                           const Values& inputs, bool enableCache);

    virtual Values doOperation(const AbstractModel* model);

    // Printable header with all columns of this operation separated by commas.
    // Format: "function_entity_attrName_value"
    virtual QString printableHeader();

    virtual bool operator==(const Output& output);

    inline Function function() const { return m_func; }
    inline Entity entity() const { return m_entity; }
    inline int attrIdx() const { return m_attrIdx; }
    inline const Values& inputs() const { return m_inputs; }

private:
    const Function m_func;
    const Entity m_entity;
    const QString m_attrName;
    const int m_attrIdx;
    const std::vector<Value> m_inputs;
};

}
#endif // UTILS_H
