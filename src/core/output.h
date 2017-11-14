/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <forward_list>
#include <vector>

#include "abstractmodel.h"
#include "attributes.h"
#include "stats.h"

namespace evoplex {

class Output
{
public:
    explicit Output(Values inputs);

    virtual void doOperation(const AbstractModel* model) = 0;

    // Printable header with all columns of this operation separated by commas.
    virtual QString printableHeader() = 0;

    virtual bool operator==(const Output* output) = 0;

    static std::vector<Output*> parseHeader(const QStringList& header,
            const Attributes& agentAttrMin, const Attributes &edgeAttrMin, QString &errorMsg);

    const int addCache(Values inputs);
    inline bool isEmpty(int cacheId) const { return m_caches.at(cacheId).rows.empty(); }
    inline const Values& readFrontRow(int cacheId) const { return m_caches.at(cacheId).rows.front(); }
    inline void flushFrontRow(int cacheId) { m_caches.at(cacheId).rows.pop_front(); }

    inline const Values& inputs() const { return m_inputs; }

protected:
    struct Cache {
        Values inputs; // columns
        std::forward_list<Values> rows;
        std::forward_list<Values>::iterator last;
    };
    std::vector<Cache> m_caches;
    Values m_inputs;

    // auxiliar method for 'doOperation()'
    void updateCaches(const Values& allValues);
};


class CustomOutput: public Output
{
public:
    explicit CustomOutput(Values inputs);

    virtual void doOperation(const AbstractModel* model);

    // Printable header with all columns of this operation separated by commas.
    // Format: "custom_nameDefinedInTheModel"
    virtual QString printableHeader();

    virtual bool operator==(const Output* output);
};


class DefaultOutput : public Output
{
public:
    enum Entity {
        E_Agents,
        E_Edges
    };

    enum Function {
        F_Invalid,
        F_Count
    };
    static std::vector<QString> availableFunctions() {
        return {"count"};
    }
    static Function funcFromString(QString f) {
        if (f == "count") return F_Count;
        return F_Invalid;
    }
    static QString stringFromFunc(Function f) {
        if (f == F_Count) return "count";
        return "invalid";
    }

    explicit DefaultOutput(Function f, Entity e, const QString& attrName, int attrIdx, Values inputs);

    virtual void doOperation(const AbstractModel* model);

    // Printable header with all columns of this operation separated by commas.
    // Format: "function_entity_attrName_value"
    virtual QString printableHeader();

    virtual bool operator==(const Output* output);

    inline Function function() const { return m_func; }
    inline Entity entity() const { return m_entity; }
    inline int attrIdx() const { return m_attrIdx; }

private:
    const Function m_func;
    const Entity m_entity;
    const QString m_attrName;
    const int m_attrIdx;
};

}
#endif // UTILS_H
