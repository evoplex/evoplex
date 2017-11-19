/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <forward_list>
#include <set>
#include <unordered_map>
#include <vector>

#include "abstractmodel.h"
#include "attributes.h"
#include "stats.h"

namespace evoplex {

class Output
{
public:
    typedef std::pair<int, Values> Row; // <rowNumber, values>

    explicit Output(Values inputs, const std::vector<int> trialIds);

    virtual void doOperation(const int trialId, const AbstractModel* model) = 0;

    // Printable header with all columns of this operation separated by commas.
    virtual QString printableHeader() = 0;

    virtual bool operator==(const Output* output) = 0;

    static std::vector<Output*> parseHeader(const QStringList& header, const std::vector<int> trialIds,
            const Attributes& agentAttrMin, const Attributes &edgeAttrMin, QString &errorMsg);

    const int addCache(Values inputs, const std::vector<int> trialIds);

    inline bool isEmpty() const
    { return m_caches.empty(); }

    inline bool isEmpty(const int cacheId, const int trialId) const
    { return m_caches.at(cacheId).trials.at(trialId).rows.empty(); }

    inline const Row& readFrontRow(const int cacheId, const int trialId) const
    { return m_caches.at(cacheId).trials.at(trialId).rows.front(); }

    inline void flushFrontRow(const int cacheId, const int trialId)
    { m_caches.at(cacheId).trials.at(trialId).rows.pop_front(); }

    // CAUTION! We trust it will NEVER be called in a running experiment.
    // Make sure it is paused first.
    void removeCache(const int cacheId, const int trialId);

    inline const Values& allInputs() const
    { return m_allInputs; }

    inline const Values& inputs(const int cacheId, const int trialId) const
    { return m_caches.at(cacheId).inputs; }

    inline const std::set<int>& trialIds() const
    { return m_trialIds; }

protected:
    struct Data {
        std::forward_list<Row> rows;
        std::forward_list<Row>::const_iterator last;
    };

    struct Cache {
        Values inputs; // columns
        std::unordered_map<int, Data> trials;
    };

    int m_lastCacheId;
    std::unordered_map<int, Cache> m_caches;
    std::set<int> m_trialIds;   // convenient to handle 'doOperation' requests
    Values m_allInputs;

    // auxiliar method for 'doOperation()'
    void updateCaches(const int trialId, const int currStep, const Values& allValues);

private:
    // auxiliar method to update the vector with all the current inputs
    void updateListOfInputs();
};


class CustomOutput: public Output
{
public:
    explicit CustomOutput(Values inputs, const std::vector<int> trialIds);

    virtual void doOperation(const int trialId, const AbstractModel* model);

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

    explicit DefaultOutput(const Function f, const Entity e, const QString& attrName,
                           const int attrIdx, Values inputs, const std::vector<int> trialIds);

    virtual void doOperation(const int trialId, const AbstractModel* model);

    // Printable header with all columns of this operation separated by commas.
    // Format: "function_entity_attrName_value"
    virtual QString printableHeader();

    virtual bool operator==(const Output* output);

    inline Function function() const { return m_func; }
    inline QString functionStr()  const { return DefaultOutput::stringFromFunc(m_func); }
    inline Entity entity() const { return m_entity; }
    inline int attrIdx() const { return m_attrIdx; }
    inline QString attrName() const { return m_attrName; }

private:
    const Function m_func;
    const Entity m_entity;
    const QString m_attrName;
    const int m_attrIdx;
};

}
#endif // UTILS_H
