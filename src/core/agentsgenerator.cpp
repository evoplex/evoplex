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
#include "utils.h"

namespace evoplex
{

AgentsGenerator::AgentsGenerator(const AttributesSpace& agentAttrsSpace)
    : m_attrsSpace(agentAttrsSpace)
{
}

QString AgentsGenerator::enumToString(Function func)
{
    switch (func) {
    case F_Min:
        return "min";
    case F_Max:
        return "max";
    case F_Rand:
        return "rand";
    case F_Value:
        return "value";
    default:
        return "invalid";
    }
}

AgentsGenerator::Function AgentsGenerator::enumFromString(const QString& funcStr)
{
    if (funcStr == "min") return F_Min;
    else if (funcStr == "max") return F_Max;
    else if (funcStr == "rand") return F_Rand;
    else if (funcStr == "value") return F_Value;
    else if (funcStr == "invalid") return F_Invalid;
}

/*********************/

AGFromFile::AGFromFile(const AttributesSpace& attrsSpace, const QString& filePath)
    : AgentsGenerator(attrsSpace)
    , m_filePath(filePath)
{
    m_command = filePath;
}

Agents AGFromFile::create(std::function<void(int)> progress)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Agent::readFromFile]: unable to read csv file with the set of agents." << m_filePath;
        return Agents();
    }

    QTextStream in(&file);

    // read and validate header
    bool hasCoords = false;
    QStringList header;
    if (!in.atEnd()) {
        bool hasCoordX = false;
        bool hasCoordY = false;
        header = in.readLine().split(",");
        for (int i = 0; i < header.size(); ++i) {
            QString attrName = header.at(i);
            for (int j = i+1; j < header.size(); ++j) {
                if (attrName == header.at(j)) {
                    qWarning() << "[Agent::readFromFile]: unable to read the set of agents from" << m_filePath
                               << QString("The headers should be unique! '%1' is duplicated.").arg(attrName);
                    return Agents();
                }
            }

            if (attrName == "x") {
                hasCoordX = true;
            } else if (attrName == "y") {
                hasCoordY = true;
            }
        }

        hasCoords = hasCoordX;
        if (hasCoordX != hasCoordY) {
            qWarning() << "[Agent::readFromFile]: unable to read the set of agents from" << m_filePath
                       << "One of the 2d coordinates are missing. Make sure you have both 'x' and 'y' columns.";
            return Agents();
        }
    }

    for (const auto* vs : m_attrsSpace) {
        if (!header.contains(vs->attrName())) {
            qWarning() << "[Agent::readFromFile]: the agents from '" << m_filePath << "' are incompatible with the model."
                       << "Expected attributes:" << m_attrsSpace.keys();
            return Agents();
        }
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
        Attributes attributes(hasCoords ? values.size()-2 : values.size());
        for (int i = 0; i < values.size(); ++i) {
            if (header.at(i) == "x") {
                coordX = values.at(i).toInt(&isValid);
            } else if (header.at(i) == "y") {
                coordY = values.at(i).toInt(&isValid);
            } else {
                ValueSpace* valSpace = m_attrsSpace.value(header.at(i));
                if (valSpace) { // is null if the column is not required
                    Value value = valSpace->validate(values.at(i));
                    if (!value.isValid()) {
                        isValid = false;
                        break;
                    }
                    attributes.replace(valSpace->id(), header.at(i), value);
                }
            }
        }
        agents.emplace_back(new Agent(id, attributes, coordX, coordY));
        progress(id);
        ++id;
    }
    file.close();

    if (isValid) {
        agents.shrink_to_fit();
    } else {
        Utils::deleteAndShrink(agents);
    }

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
        Q_ASSERT(funcInput.type() == Value::INT);
        m_command = QString("*%1;rand_%2").arg(m_numAgents).arg(funcInput.toQString());
        m_prg = new PRG(funcInput.toInt());
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

Agents AGSameFuncForAll::create(std::function<void(int)> progress)
{
    Agents agents;
    agents.reserve(m_numAgents);
    for (int agentId = 0; agentId < m_numAgents; ++agentId) {
        Attributes attrs(m_attrsSpace.size());
        for (ValueSpace* valSpace : m_attrsSpace) {
            attrs.replace(valSpace->id(), valSpace->attrName(), f_value(valSpace));
        }
        agents.emplace_back(new Agent(agentId, attrs));
        progress(agentId);
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
    Q_ASSERT(m_numAgents > 0);
    m_command = QString("#%1").arg(m_numAgents);
    for (const AttrCmd& cmd : m_attrCmds) {
        m_command += QString(";%1_%2").arg(cmd.attrName).arg(enumToString(cmd.func));
        if (cmd.funcInput.isValid()) {
            m_command += "_" + cmd.funcInput.toQString();
        }
    }
}

Agents AGDiffFunctions::create(std::function<void(int)> progress)
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
            prg = new PRG(cmd.funcInput.toInt());
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
        delete prg;
    }

    Agents agents;
    agents.reserve(m_numAgents);
    for (int agentId = 0; agentId < m_numAgents; ++agentId) {
        agents.emplace_back(new Agent(agentId, agentsAttrs.at(agentId)));
        progress(agentId);
    }
    return agents;
}

/*********************/

bool AgentsGenerator::saveToFile(QString& filePath, Agents agents, std::function<void(int)>& progress)
{
    if (agents.empty()) {
        qWarning() << "[AgentsGenerator]: Tried to save an empty set of agents.";
        return false;
    }

    if (!filePath.endsWith(".csv")) {
        filePath += ".csv";
    }

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "[AgentsGenerator]: Failed to save the set of agents to file." << filePath;
        return false;
    }

    QTextStream out(&file);
    const std::vector<QString>& header = agents.front()->attrs().names();
    for (const QString& col : header) {
        out << col << ",";
    }
    out << "x,y\n";

    for (Agent* agent : agents) {
        for (const Value& value : agent->attrs().values()) {
            out << value.toQString() << ",";
        }
        out << agent->x() << ",";
        out << agent->y() << "\n";

        out.flush();
        progress(agent->id());
    }

    file.close();
    return true;
}

AgentsGenerator* AgentsGenerator::parse(const AttributesSpace& agentAttrsSpace,
                                        const QString& command, QString& errMsg)
{
    AgentsGenerator* ag = nullptr;

    if (QFileInfo::exists(command)) {
        ag = new AGFromFile(agentAttrsSpace, command);
    } else {
        QStringList cmds = command.split(";");
        if (cmds.size() < 2) {
            errMsg = QString("The command %1 is invalid!").arg(command);
            qWarning() << "[Agent::createAgents()]:" << errMsg;
            return nullptr;
        }

        bool ok;
        QString numAgentsStr = cmds.at(0);
        const int numAgents = numAgentsStr.remove(0,1).toInt(&ok);
        if (!ok) {
            errMsg = QString("Unable to parse '%1'."
                    "\n'%2' should be an integer representing the number of agents.")
                    .arg(command).arg(numAgentsStr);
            qWarning() << "[Agent::createAgents()]:" << errMsg;
            return nullptr;
        }

        if (command.startsWith("*")) {
            if (cmds.size() != 2) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '*numAgents;[min|max|rand_seed]'")
                        .arg(command).arg(numAgentsStr);
                qWarning() << "[Agent::createAgents()]:" << errMsg;
                return nullptr;
            }

            Function func = enumFromString(cmds.at(1));
            Value value;

            if (cmds.at(1).startsWith("rand_")) {
                func = F_Rand;
                QString seedStr = cmds.at(1);
                value = Value(seedStr.remove("rand_").toInt(&ok)); // seed
                if (!ok) {
                    errMsg = QString("Unable to parse '%1'."
                            "It should look like: '*numAgents;rand_seed'")
                            .arg(command);
                    qWarning() << "[Agent::createAgents()]:" << errMsg;
                    return nullptr;
                }
            } else if (func == F_Invalid) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '*numAgents;[min|max|rand_seed]'")
                        .arg(command);
                qWarning() << "[Agent::createAgents()]:" << errMsg;
                return nullptr;
            }
            ag = new AGSameFuncForAll(agentAttrsSpace, numAgents, func, value);
        } else if (command.startsWith("#")) {
            cmds.removeFirst();
            if (cmds.size() != agentAttrsSpace.size()) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '#numAgents;attrName_[min|max|rand_seed|value_value]'"
                        "and must contain all attributes of the current model (i.e., '%2')")
                        .arg(command).arg(agentAttrsSpace.keys().join(", "));
                qWarning() << "[Agent::createAgents()]:" << errMsg;
                return nullptr;
            }

            std::vector<AGDiffFunctions::AttrCmd> attrCmds;
            attrCmds.reserve(cmds.size());

            for (const QString& cmd : cmds) {
                AGDiffFunctions::AttrCmd attrCmd;
                QStringList attrCmdStr = cmd.split("_");

                attrCmd.attrName = attrCmdStr.at(0);
                const ValueSpace* valSpace = agentAttrsSpace.value(attrCmd.attrName, nullptr);
                if (valSpace) {
                    attrCmd.attrId = valSpace->id();
                } else {
                    errMsg = QString("Unable to parse '%1'."
                            "The attribute '%2' does not belong to the current model.")
                            .arg(command).arg(attrCmd.attrName);
                    qWarning() << "[Agent::createAgents()]:" << errMsg;
                    return nullptr;
                }

                attrCmd.func = enumFromString(attrCmdStr.at(1));
                if (attrCmd.func == F_Invalid) {
                    errMsg = QString("Unable to parse '%1'."
                                "The function '%2' is invalid.")
                                .arg(command).arg(attrCmdStr.at(1));
                    qWarning() << "[Agent::createAgents()]:" << errMsg;
                    return nullptr;
                } else if (attrCmd.func == F_Rand) {
                    attrCmd.funcInput = Value(attrCmdStr.at(2).toInt(&ok)); // seed
                    if (!ok) {
                        errMsg = QString("Unable to parse '%1'. The PRG seed should be an integer!").arg(command);
                        qWarning() << "[Agent::createAgents()]:" << errMsg;
                        return nullptr;
                    }
                } else if (attrCmd.func == F_Value){
                    attrCmd.funcInput = agentAttrsSpace.value(attrCmd.attrName)->validate(attrCmdStr.at(2));
                    if (!attrCmd.funcInput.isValid()) {
                        errMsg = QString("Unable to parse '%1'. The value is invalid!").arg(command);
                        qWarning() << "[Agent::createAgents()]:" << errMsg;
                        return nullptr;
                    }
                }

                attrCmds.emplace_back(attrCmd);
            }

            ag = new AGDiffFunctions(agentAttrsSpace, numAgents, attrCmds);
        }
    }

    if (!ag) {
        errMsg = QString("the command '%1'. is invalid!").arg(command);
        qWarning() << "[Agent::createAgents()]:" << errMsg;
        return nullptr;
    }

    Q_ASSERT(command == ag->command());
    return ag;
}


} // evoplex
