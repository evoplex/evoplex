/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OUTPUT_H
#define OUTPUT_H

#include <forward_list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "attributes.h"
#include "attributerange.h"
#include "modelplugin.h"
#include "stats.h"

namespace evoplex
{

class Output;
class CustomOutput;
class DefaultOutput;

typedef std::shared_ptr<Output> OutputPtr;
typedef std::shared_ptr<CustomOutput> CustomOutputPtr;
typedef std::shared_ptr<DefaultOutput> DefaultOutputPtr;

class Cache
{
    friend class Output;
public:
    typedef std::pair<int, Values> Row; // <rowNumber, values>

    bool isEmpty(const int trialId) const;

    void deleteCache();

    QString printableHeader(const char sep, const bool joinInputs) const;

    inline OutputPtr output() const { return m_parent; }
    inline const Values& inputs() const { return m_inputs; }
    inline const Row& readFrontRow(const int trialId) const { return m_trials.at(trialId).rows.front(); }
    inline void flushFrontRow(const int trialId) { m_trials.at(trialId).rows.pop_front(); }
    void flushAll();

private:
    struct Data {
        std::forward_list<Row> rows;
        std::forward_list<Row>::const_iterator last;
    };

    OutputPtr m_parent;
    Values m_inputs; // columns
    std::unordered_map<int, Data> m_trials;

    // let's keep it private to ensure that only Output can create a Cache
    explicit Cache(const Values& inputs, const std::vector<int>& trialIds, OutputPtr parent);
};

class Output : public std::enable_shared_from_this<Output>
{
public:
    static std::vector<Cache*> parseHeader(const QStringList& header,
        const std::vector<int>& trialIds, const ModelPlugin* model, QString& errorMsg);

    static QString printableHeader(const QString& prefix, const Values& inputs,
                                   const char sep, const bool joinInputs);

    virtual ~Output();

    virtual void doOperation(const Trial* trial) = 0;

    // Printable header with all columns of this operation separated by 'sep'.
    // If joinInputs is enabled: eg: func_attr_input1[sep]input2
    // If joinInputs is disabled: eg: func_attr_input1[sep]func_attr_input2
    QString printableHeader(const char sep, const bool joinInputs) const;

    // Format for CustomOutput: "custom_nameDefinedInTheModel"
    // Format for DefaultOutput: "function_entity_attrName_value"
    const QString& printableHeaderPrefix() const { return m_headerPrefix; }

    virtual bool operator==(const OutputPtr output) const = 0;

    Cache* addCache(const Values& inputs, const std::vector<int>& trialIds);

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

    virtual void doOperation(const Trial* trial);

    virtual bool operator==(const OutputPtr output) const;
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

    explicit DefaultOutput(Function f, Entity e, AttributeRangePtr attrRange);

    virtual void doOperation(const Trial* trial);

    virtual bool operator==(const OutputPtr output) const;

    inline Function function() const { return m_func; }
    inline QString functionStr()  const { return DefaultOutput::stringFromFunc(m_func); }
    inline Entity entity() const { return m_entity; }
    inline const AttributeRangePtr attrRange() const { return m_attrRange; }

private:
    const Function m_func;
    const Entity m_entity;
    const AttributeRangePtr m_attrRange;
};

}
#endif // UTILS_H
