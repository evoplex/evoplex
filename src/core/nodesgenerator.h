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

#ifndef NODESGENERATOR_H
#define NODESGENERATOR_H

#include <functional>

#include "attributerange.h"
#include "node.h"
#include "modelplugin.h"

namespace evoplex
{

class NodesGenerator
{
public:
    enum Mode {
        M_Invalid,
        M_FromFile,
        M_SameFunctionForAll,
        M_DifferentFunctions
    };

    enum Function {
        F_Invalid,
        F_Min,
        F_Max,
        F_Rand,
        F_Value
    };
    static QString enumToString(Function func);
    static Function enumFromString(const QString& funcStr);

    // Expected commands:
    //   path to a csv file
    //     'filepath'
    //   same mode for all attributes
    //     '*numNodes;[min|max|rand_seed]'
    //   specific mode for each attribute
    //     '#numNodes;attrName_[min|max|rand_seed|value_val];...'
    static NodesGenerator* parse(const AttributesScope& nodeAttrsScope,
                                  const QString& command, QString& errMsg);

    // Export set of nodes to a csv file
    static bool saveToFile(QString& filepath, Nodes nodes, std::function<void(int)>& progress);

    virtual ~NodesGenerator() {}

    virtual Nodes create(std::function<void(int)> progress = [](int){}) = 0;

    inline const QString& command() { return m_command; }

protected:
    explicit NodesGenerator(const AttributesScope &nodeAttrsScope);

    const AttributesScope m_attrsScope;
    QString m_command;
};

// Import a set of nodes from a csv file
class AGFromFile : public NodesGenerator
{
public:
    explicit AGFromFile(const AttributesScope& attrsScope, const QString& filePath);
    Nodes create(std::function<void(int)> progress = [](int){});
    inline const QString& filePath() const { return m_filePath; }
private:
    const QString m_filePath;
};

// using the same function for all node attribute
class AGSameFuncForAll : public NodesGenerator
{
public:
    explicit AGSameFuncForAll(const AttributesScope& attrsScope, const int numNodes,
                              const Function& func, const Value& funcInput);
    ~AGSameFuncForAll();
    Nodes create(std::function<void(int)> progress = [](int){});
    inline int numNodes() const { return m_numNodes; }
    inline Function function() const { return m_function; }
    inline const Value& functionInput() const { return m_functionInput; }
private:
    const int m_numNodes;
    const Function m_function;
    const Value m_functionInput;

    std::function<Value(const AttributeRange*)> f_value;
    PRG* m_prg;
};

// using different functions for each node attribute
class AGDiffFunctions : public NodesGenerator
{
public:
    struct AttrCmd {
        int attrId;
        QString attrName;
        Function func;
        Value funcInput;
    };

    explicit AGDiffFunctions(const AttributesScope& attrsScope, const int numNodes,
                             std::vector<AttrCmd> attrCmds);
    Nodes create(std::function<void(int)> progress = [](int){});
    inline int numNodes() const { return m_numNodes; }
    inline const std::vector<AttrCmd>& attrCmds() const { return m_attrCmds; }
private:
    const int m_numNodes;
    const std::vector<AttrCmd> m_attrCmds;
};

} // evoplex
#endif // NODESGENERATOR_H
