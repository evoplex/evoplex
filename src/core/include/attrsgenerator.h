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

#ifndef ATTRS_GENERATOR_H
#define ATTRS_GENERATOR_H

#include <functional>
#include <memory>

#include "attributes.h"
#include "attributerange.h"
#include "enum.h"

namespace evoplex {

class AGSameFuncForAll;
class AGDiffFunctions;
class AttrsGenerator;
using AttrsGeneratorPtr = std::unique_ptr<AttrsGenerator>;

class AttrsGeneratorInterface
{
public:
    virtual ~AttrsGeneratorInterface() = default;
    virtual SetOfAttributes create(int size=-1,
        std::function<void(int)> progress = [](int){}) = 0;
};

/**
 * @brief Generates a set of Attributes
 */
class AttrsGenerator : public AttrsGeneratorInterface
{
public:
    // Expected commands:
    //     - integer
    //         '*integer;min' alias
    //     - same function for all attributes:
    //         '*integer;[min|max|rand_seed]'
    //         '*[min|max|rand_seed]'
    //              -> alias for interger=1
    //     - specific function for each attribute:
    //         '#integer;attrName_[min|max|rand_seed|value_val];...'
    //         '#attrName_[min|max|rand_seed|value_val];...'
    //              -> alias for interger=1
    //     * the integer corresponds to the size of the set of attributes
    static AttrsGeneratorPtr parse(const AttributesScope& attrsScope,
                                   const QString &cmd, QString& error);

    virtual ~AttrsGenerator() = default;

    inline AttributesScope attrsScope() const { return m_attrsScope; }

    // the source command for the AttrsGenerator object
    inline const QString& command() { return m_command; }

    // the size of the set of attributes
    inline int size() const { return m_size; }

    // given a 'rand_seed', if seed is a positive int, return Value(seed)
    // else, it returns an invalid Value
    static Value parseRandSeed(QString seedStr);

protected:
    const AttributesScope m_attrsScope;
    const int m_size;
    QString m_command;

    explicit AttrsGenerator(const AttributesScope& attrsScope, const int numCopies);

private:
    // auxiliar parser for commands starting with '*'
    static std::unique_ptr<AGSameFuncForAll> parseStarCmd(
            const AttributesScope& attrsScope, const int numCopies,
            const QStringList& cmds, QString& error);

    // auxiliar parser for commands starting with '#'
    static std::unique_ptr<AGDiffFunctions> parseHashCmd(
            const AttributesScope& attrsScope, const int numCopies,
            const QStringList& cmds, QString& error);
};


// using the same function for all attributes
class AGSameFuncForAll : public AttrsGenerator
{
public:
    explicit AGSameFuncForAll(const AttributesScope& attrsScope, const int numCopies,
                              const Function& func, const Value& funcInput);
    ~AGSameFuncForAll() override;

    SetOfAttributes create(int size=-1,
            std::function<void(int)> progress = [](int){}) override;

    inline Function function() const { return m_function; }
    inline const Value& functionInput() const { return m_functionInput; }

private:
    const Function m_function;
    const Value m_functionInput;

    std::function<Value(AttributeRangePtr)> f_value;
    PRG* m_prg;
};


// using different functions for each attribute
class AGDiffFunctions : public AttrsGenerator
{
public:
    struct AttrCmd {
        int attrId;
        QString attrName;
        Function func;
        Value funcInput;
    };

    explicit AGDiffFunctions(const AttributesScope& attrsScope, const int numCopies,
                             const std::vector<AttrCmd>& attrCmds);
    ~AGDiffFunctions() override = default;

    SetOfAttributes create(int size=-1,
            std::function<void(int)> progress = [](int){}) override;

    inline const std::vector<AttrCmd>& attrCmds() const { return m_attrCmds; }

private:
    const std::vector<AttrCmd> m_attrCmds;
};

} // evoplex
#endif // ATTRS_GENERATOR_H
