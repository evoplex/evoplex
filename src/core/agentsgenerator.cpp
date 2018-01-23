/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>
#include <functional>

#include "agentsgenerator.h"
#include "modelplugin.h"

namespace evoplex
{

AgentsGenerator::AgentsGenerator(const AttributesSpace& agentAttrsSpace)
    : m_attrsSpace(agentAttrsSpace)
{
}

/*********************/

AGFromFile::AGFromFile(const AttributesSpace& attrsSpace, const QString& filePath)
    : AgentsGenerator(attrsSpace)
    , m_filePath(filePath)
{
    m_command = filePath;
}

Agents AGFromFile::create()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Agent::readFromFile]: unable to read csv file with the set of agents." << m_filePath;
        return Agents();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        foreach (QString attrName, header) {
            if (attrName != "x" && attrName != "y" && !m_attrsSpace.contains(attrName)) {
                header.clear();
                break;
            }
        }
    }

    if (header.isEmpty()) {
        qWarning() << "[Agent::readFromFile]: unable to read the set of agents from" << m_filePath
                   << "Expected properties:" << m_attrsSpace.keys();
        return Agents();
    }

    // create agents
    int id = 0;
    Agents agents;
    bool isValid = true;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[Agent::readFromFile]: rows must have the same number of columns!";
            isValid = false;
            break;
        }

        int coordX = 0;
        int coordY = id;
        Attributes attributes(values.size());
        for (int i = 0; i < values.size(); ++i) {
            if (header.at(i) == "x") {
                coordX = values.at(i).toInt(&isValid);
            } else if (header.at(i) == "y") {
                coordY = values.at(i).toInt(&isValid);
            } else {
                ValueSpace* valSpace = m_attrsSpace.value(header.at(i));
                Value value = valSpace->validate(values.at(i));
                if (!value.isValid()) {
                    isValid = false;
                    break;
                }
                attributes.replace(valSpace->id(), header.at(i), value);
            }
        }
        agents.emplace_back(new Agent(id, attributes, coordX, coordY));
        ++id;
    }
    file.close();

    if (!isValid) {
        qDeleteAll(agents);
        agents.clear();
    }
    agents.shrink_to_fit();

    return agents;
}

/*********************/

AGSameFuncForAll::AGSameFuncForAll(const AttributesSpace& attrsSpace, const int numAgents,
                                   const Function& func, const Value& funcInput)
    : AgentsGenerator(attrsSpace)
    , m_numAgents(numAgents)
    , m_function(func)
    , m_functionInput(funcInput)
    , m_prg(nullptr)
{
    Q_ASSERT(m_numAgents > 0 && m_function != F_Invalid);

    switch (m_function) {
    case F_Min:
        m_command = QString("*%1;min").arg(m_numAgents);
        f_value = [](const ValueSpace* valSpace) { return valSpace->min(); };
        break;
    case F_Max:
        m_command = QString("*%1;max").arg(m_numAgents);
        f_value = [](const ValueSpace* valSpace) { return valSpace->max(); };
        break;
    case F_Rand:
        Q_ASSERT(funcInput.type == Value::INT);
        m_command = QString("*%1;rand_%2").arg(m_numAgents).arg(funcInput.toQString());
        f_value = [this](const ValueSpace* valSpace) { return valSpace->rand(m_prg); };
        break;
    default:
        qFatal("[AGSameFuncForAll]: invalid function!");
        break;
    }
}

AGSameFuncForAll::~AGSameFuncForAll()
{
    delete m_prg;
}

Agents AGSameFuncForAll::create()
{
    Agents agents;
    for (int agentId = 0; agentId < m_numAgents; ++agentId) {
        Attributes attrs(m_attrsSpace.size());
        for (ValueSpace* valSpace : m_attrsSpace) {
            attrs.replace(valSpace->id(), valSpace->attrName(), f_value(valSpace));
        }
        agents.emplace_back(new Agent(agentId, attrs));
    }
    return agents;
}

/*********************/

AGDiffFunctions::AGDiffFunctions(const AttributesSpace &attrsSpace, const int numAgents,
                                 std::vector<AttrCmd> attrCmds)
    : AgentsGenerator(attrsSpace)
    , m_numAgents(numAgents)
    , m_attrCmds(attrCmds)
{

}

Agents AGDiffFunctions::create()
{
    std::vector<Attributes> agentsAttrs;
    agentsAttrs.reserve(m_numAgents);
    for (int i = 0; i < m_numAgents; ++i) {
        Attributes attrs(m_attrsSpace.size());
        agentsAttrs.emplace_back(attrs);
    }

    std::function<Value()> value;
    for (const AttrCmd& cmd : m_attrCmds) {
        const ValueSpace* valSpace = m_attrsSpace.value(cmd.attrName, nullptr);
        Q_ASSERT(valSpace);

        PRG* prg = nullptr;
        switch (cmd.func) {
        case F_Min:
            value = [valSpace]() { return valSpace->min(); };
            break;
        case F_Max:
            value = [valSpace]() { return valSpace->max(); };
            break;
        case F_Rand:
            prg = new PRG(cmd.funcInput.toInt);
            value = [valSpace, prg]() { return valSpace->rand(prg); };
            break;
        case F_Value:
            value = [cmd]() { return cmd.funcInput; };
            break;
        default:
            qFatal("[AGDiffFunctions]: invalid function!");
        }

        for (Attributes& attrs : agentsAttrs) {
            attrs.replace(valSpace->id(), valSpace->attrName(), value());
        }
    }

    Agents agents;
    agents.reserve(m_numAgents);
    for (int agentId = 0; agentId < m_numAgents; ++agentId) {
        agents.emplace_back(new Agent(agentId, agentsAttrs.at(agentId)));
    }
    return agents;
}

/*********************/

AgentsGenerator* AgentsGenerator::parse(const AttributesSpace& agentAttrsSpace, const QString& command)
{
    AgentsGenerator* ag = nullptr;

    if (QFileInfo::exists(command)) {
        ag = new AGFromFile(agentAttrsSpace, command);
    } else {
        QStringList cmds = command.split(";");
        if (cmds.size() < 2) {
            qWarning() << "[Agent::createAgents()]: the command"
                       << command << "is invalid!";
            return nullptr;
        }

        bool ok;
        QString numAgentsStr = cmds.at(0);
        const int numAgents = numAgentsStr.remove(0,1).toInt(&ok);
        if (!ok) {
            QString err = QString("[Agent::createAgents()]: unable to parse '%1'."
                                  "'%2' should be an integer representing the number of agents."
                                  ).arg(command).arg(numAgentsStr);
            qWarning() << err;
            return nullptr;
        }

        if (command.startsWith("*")) {
            if (cmds.size() != 2) {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The command should looks like: '*numAgents;[min|max|rand_seed]'";
                return nullptr;
            }

            if (cmds.at(1) == "min") {
                ag = new AGSameFuncForAll(agentAttrsSpace, numAgents, F_Min, Value());
            } else if (cmds.at(1) == "max") {
                ag = new AGSameFuncForAll(agentAttrsSpace, numAgents, F_Max, Value());
            } else if (cmds.at(1).startsWith("rand_")) {
                QString seedStr = cmds.at(1);
                int seed = seedStr.remove("rand_").toInt(&ok);
                if (!ok) {
                    qWarning() << "[Agent::createAgents()]: unable to parse" << command
                               << "The command should looks like: '*numAgents;rand_seed'";
                    return nullptr;
                }
                ag = new AGSameFuncForAll(agentAttrsSpace, numAgents, F_Rand, Value(seed));
            } else {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The command should looks like: '*numAgents;[min|max|rand_seed]'";
                return nullptr;
            }
        } else if (command.startsWith("#")) {
            cmds.removeFirst();
            if (cmds.size() != agentAttrsSpace.size()) {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The command should looks like: '#numAgents;attrName_[min|max|rand_seed|value_value]'"
                           << "and must contain all attributes of the current model (i.e., '"
                           << agentAttrsSpace.keys() << "').";
                return nullptr;
            }

            std::vector<AGDiffFunctions::AttrCmd> attrCmds;
            attrCmds.reserve(cmds.size());

            for (const QString& cmd : cmds) {
                AGDiffFunctions::AttrCmd attrCmd;
                QStringList attrCmdStr = cmd.split("_");

                attrCmd.attrName = attrCmdStr.first();
                if (!agentAttrsSpace.contains(attrCmd.attrName)) {
                    qWarning() << "[Agent::createAgents()]: unable to parse" << command
                               << "The attribute '" << attrCmd.attrName << "' does not belong to the model.";
                    return nullptr;
                } else if (attrCmdStr.size() == 2) {
                    if (attrCmdStr.at(1) == "min") {
                        attrCmd.func = F_Min;
                    } else if (attrCmdStr.at(1) == "max") {
                        attrCmd.func = F_Max;
                    } else {
                        qWarning() << "[Agent::createAgents()]: unable to parse" << command
                                   << "The command " << attrCmdStr.at(1) << "should be 'min' or 'max'";
                        return nullptr;
                    }
                } else if (attrCmdStr.size() == 3) {
                    if (attrCmdStr.at(1) == "rand") {
                        int seed = attrCmdStr.at(2).toInt(&ok);
                        if (!ok) {
                            qWarning() << "[Agent::createAgents()]: unable to parse" << command
                                       << "The command should looks like: '#numAgents;attrName_rand_seed;...'";
                            return nullptr;
                        }
                        attrCmd.func = F_Rand;
                        attrCmd.funcInput = Value(seed);
                    } else if (attrCmdStr.at(1) == "value"){
                        attrCmd.func = F_Value;
                        attrCmd.funcInput = agentAttrsSpace.value(attrCmd.attrName)->validate(attrCmdStr.at(2));
                    } else {
                        qWarning() << "[Agent::createAgents()]: unable to parse" << command
                                   << "The command " << attrCmdStr.at(1) << "should be 'rand' or 'value'";
                        return nullptr;
                    }
                } else {
                    qWarning() << "[Agent::createAgents()]: unable to parse" << command
                               << "The command " << cmd
                               << "should looks like: attrName_[min|max|rand_seed|value_value]";
                    return nullptr;
                }

                attrCmds.emplace_back(attrCmd);
            }

            ag = new AGDiffFunctions(agentAttrsSpace, numAgents, attrCmds);
        }
    }

    if (!ag) {
        qWarning() << "[Agent::createAgents()]: the command" << command << "is invalid!";
        return nullptr;
    }

    Q_ASSERT(command == ag->command());
    return ag;
}


} // evoplex
