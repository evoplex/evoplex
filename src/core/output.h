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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <forward_list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "abstractmodel.h"
#include "attributes.h"
#include "modelplugin.h"
#include "stats.h"

namespace evoplex
{

class Output;
class CustomOutput;
class DefaultOutput;

typedef std::shared_ptr<Output> OutputSP;
typedef std::shared_ptr<CustomOutput> CustomOutputSP;
typedef std::shared_ptr<DefaultOutput> DefaultOutputSP;

class Cache
{
    friend class Output;
public:
    typedef std::pair<int, Values> Row; // <rowNumber, values>

    bool isEmpty(const int trialId) const;

    void deleteCache();

    QString printableHeader(const char sep, const bool joinInputs) const;

    inline OutputSP output() const { return m_parent; }
    inline const Values& inputs() const { return m_inputs; }
    inline const Row& readFrontRow(const int trialId) const { return m_trials.at(trialId).rows.front(); }
    inline void flushFrontRow(const int trialId) { m_trials.at(trialId).rows.pop_front(); }
    void flushAll();

private:
    struct Data {
        std::forward_list<Row> rows;
        std::forward_list<Row>::const_iterator last;
    };

    OutputSP m_parent;
    Values m_inputs; // columns
    std::unordered_map<int, Data> m_trials;

    // let's keep it private to ensure that only Output can create a Cache
    explicit Cache(Values inputs, std::vector<int> trialIds, OutputSP parent);
};

class Output : public std::enable_shared_from_this<Output>
{
public:
    static std::vector<Cache*> parseHeader(const QStringList& header,
        const std::vector<int> trialIds, const ModelPlugin* model, QString& errorMsg);

    static QString printableHeader(const QString& prefix, const Values& inputs,
                                   const char sep, const bool joinInputs);

    virtual ~Output();

    virtual void doOperation(const int trialId, const AbstractModel* model) = 0;

    // Printable header with all columns of this operation separated by 'sep'.
    // If joinInputs is enabled: eg: func_attr_input1[sep]input2
    // If joinInputs is disabled: eg: func_attr_input1[sep]func_attr_input2
    QString printableHeader(const char sep, const bool joinInputs) const;

    // Format for CustomOutput: "custom_nameDefinedInTheModel"
    // Format for DefaultOutput: "function_entity_attrName_value"
    const QString& printableHeaderPrefix() const { return m_headerPrefix; }

    virtual bool operator==(const OutputSP output) const = 0;

    Cache* addCache(Values inputs, const std::vector<int> trialIds);

    // CAUTION! We trust it will NEVER be called in a running experiment.
    // Make sure it is paused first.
    void deleteCache(Cache* cache);

    // flushes all its child caches
    void flushAll();

    inline const std::vector<Cache*>& caches() const { return m_caches; }
    inline bool isEmpty() const { return m_caches.empty(); }
    inline const Values& allInputs() const { return m_allInputs; }
    inline const std::set<int>& trialIds() const { return m_allTrialIds; }

protected:
    QString m_headerPrefix;
    std::vector<Cache*> m_caches; // child caches
    std::set<int> m_allTrialIds;  // convenient to handle 'doOperation' requests
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
    explicit CustomOutput();

    virtual void doOperation(const int trialId, const AbstractModel* model);

    virtual bool operator==(const OutputSP output) const;
};


class DefaultOutput : public Output
{
public:
    enum Entity {
        E_Nodes,
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

    explicit DefaultOutput(const Function f, const Entity e, const ValueSpace* valSpace);

    virtual void doOperation(const int trialId, const AbstractModel* model);

    virtual bool operator==(const OutputSP output) const;

    inline Function function() const { return m_func; }
    inline QString functionStr()  const { return DefaultOutput::stringFromFunc(m_func); }
    inline Entity entity() const { return m_entity; }
    inline const ValueSpace* valueSpace() const { return m_valueSpace; }

private:
    const Function m_func;
    const Entity m_entity;
    const ValueSpace* m_valueSpace;
};

}
#endif // UTILS_H
