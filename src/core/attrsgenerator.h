/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef ATTRS_GENERATOR_H
#define ATTRS_GENERATOR_H

#include "attributerange.h"
#include "modelplugin.h"

namespace evoplex {

class AGSameFuncForAll;
class AGDiffFunctions;

class AttrsGeneratorInterface
{
public:
    virtual ~AttrsGeneratorInterface() = default;
    virtual SetOfAttributes create(std::function<void(int)> progress = [](int){}) = 0;
};

/**
 * @brief Generates a set of Attributes
 */
class AttrsGenerator : public AttrsGeneratorInterface
{
public:
    // Expected commands:
    //     - same mode for all attributes:
    //         '*integer;[min|max|rand_seed]'
    //     - specific mode for each attribute:
    //         '#integer;attrName_[min|max|rand_seed|value_val];...'
    //     * the integer corresponds to the size of the set of attributes
    static AttrsGenerator* parse(const AttributesScope& attrsScope,
                                 const QString& cmd, QString& error);

    virtual ~AttrsGenerator();

    // the source command for the AttrsGenerator object
    inline const QString& command() { return m_command; }

    // the size of the set of attributes
    inline int size() const { return m_size; }

protected:
    const AttributesScope m_attrsScope;
    const int m_size;
    QString m_command;

    explicit AttrsGenerator(const AttributesScope& attrsScope, const int numCopies);

private:
    // auxiliar parser for commands starting with '*'
    static AGSameFuncForAll* parseStarCmd(const AttributesScope& attrsScope,
            const int numCopies, const QStringList& cmds, QString& error);

    // auxiliar parser for commands starting with '#'
    static AGDiffFunctions* parseHashCmd(const AttributesScope& attrsScope,
            const int numCopies, const QStringList& cmds, QString& error);
};


// using the same function for all attributes
class AGSameFuncForAll : public AttrsGenerator
{
public:
    explicit AGSameFuncForAll(const AttributesScope& attrsScope, const int numCopies,
                              const Function& func, const Value& funcInput);
    ~AGSameFuncForAll() override;

    SetOfAttributes create(std::function<void(int)> progress = [](int){}) override;
    inline Function function() const { return m_function; }
    inline const Value& functionInput() const { return m_functionInput; }

private:
    const Function m_function;
    const Value m_functionInput;

    std::function<Value(const AttributeRange*)> f_value;
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
                             std::vector<AttrCmd> attrCmds);
    ~AGDiffFunctions() override = default;

    SetOfAttributes create(std::function<void(int)> progress = [](int){}) override;
    inline const std::vector<AttrCmd>& attrCmds() const { return m_attrCmds; }

private:
    const std::vector<AttrCmd> m_attrCmds;
};

} // evoplex
#endif // ATTRS_GENERATOR_H
