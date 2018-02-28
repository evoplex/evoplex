/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef AGENTGENERATOR_H
#define AGENTGENERATOR_H

#include <functional>

#include "agent.h"
#include "modelplugin.h"
#include "valuespace.h"

namespace evoplex
{

class AgentsGenerator
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
    //     '*numAgents;[min|max|rand_seed]'
    //   specific mode for each attribute
    //     '#numAgents;attrName_[min|max|rand_seed|value_val];...'
    static AgentsGenerator* parse(const AttributesSpace& agentAttrsSpace,
                                  const QString& command, QString& errMsg);

    // Export set of agents to a csv file
    static bool saveToFile(QString& filepath, Agents agents, std::function<void(int)>& progress);

    virtual Agents create(std::function<void(int)> progress = [](int){}) = 0;

    inline const QString& command() { return m_command; }

protected:
    explicit AgentsGenerator(const AttributesSpace& agentAttrsSpace);

    const AttributesSpace m_attrsSpace;
    QString m_command;
};

// Import a set of agents from a csv file
class AGFromFile : public AgentsGenerator
{
public:
    explicit AGFromFile(const AttributesSpace& attrsSpace, const QString& filePath);
    Agents create(std::function<void(int)> progress = [](int){});
    inline const QString& filePath() const { return m_filePath; }
private:
    const QString m_filePath;
};

// using the same function for all agent attribute
class AGSameFuncForAll : public AgentsGenerator
{
public:
    explicit AGSameFuncForAll(const AttributesSpace& attrsSpace, const int numAgents,
                              const Function& func, const Value& funcInput);
    ~AGSameFuncForAll();
    Agents create(std::function<void(int)> progress = [](int){});
    inline const int numAgents() const { return m_numAgents; }
    inline const Function function() const { return m_function; }
    inline const Value& functionInput() const { return m_functionInput; }
private:
    const int m_numAgents;
    const Function m_function;
    const Value m_functionInput;

    std::function<Value(const ValueSpace*)> f_value;
    PRG* m_prg;
};

// using different functions for each agent attribute
class AGDiffFunctions : public AgentsGenerator
{
public:
    struct AttrCmd {
        int attrId;
        QString attrName;
        Function func;
        Value funcInput;
    };

    explicit AGDiffFunctions(const AttributesSpace& attrsSpace, const int numAgents,
                             std::vector<AttrCmd> attrCmds);
    Agents create(std::function<void(int)> progress = [](int){});
    inline const int numAgents() const { return m_numAgents; }
    inline const std::vector<AttrCmd> attrCmds() const { return m_attrCmds; }
private:
    const int m_numAgents;
    const std::vector<AttrCmd> m_attrCmds;
};

} // evoplex
#endif // AGENTGENERATOR_H
