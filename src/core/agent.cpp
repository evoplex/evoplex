/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>
#include <functional>

#include "agent.h"
#include "modelplugin.h"

namespace evoplex
{

Agents Agent::createAgents(const QString& command, const ModelPlugin* modelPlugin)
{
    if (QFileInfo::exists(command)) {
        return readFromFile(command, modelPlugin);
    }

    QStringList cmds = command.split(";");
    if (cmds.size() < 2) {
        qWarning() << "[Agent::createAgents()]: the command"
                   << command << "is invalid!";
        return Agents();
    }

    bool ok;
    QString numAgentsStr = cmds.at(0);
    const int numAgents = numAgentsStr.remove(0,1).toInt(&ok);
    if (!ok) {
        QString err = QString("[Agent::createAgents()]: unable to parse '%1'."
                              "'%2' should be an integer representing the number of agents."
                              ).arg(command).arg(numAgentsStr);
        qWarning() << err;
        return Agents();
    }

    if (command.startsWith("*")) {
        if (cmds.size() != 2) {
            qWarning() << "[Agent::createAgents()]: unable to parse" << command
                       << "The command should looks like: '*numAgents;[min|max|rand_seed]'";
            return Agents();
        }

        std::function<Value(const ValueSpace*)> value;
        if (cmds.at(1) == "min") {
            value = [](const ValueSpace* valSpace) { return valSpace->min(); };
        } else if (cmds.at(1) == "max") {
            value = [](const ValueSpace* valSpace) { return valSpace->max(); };
        } else if (cmds.at(1).startsWith("rand_")) {
            QString seedStr = cmds.at(1);
            int seed = seedStr.remove("rand_").toInt(&ok);
            if (!ok) {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The command should looks like: '*numAgents;rand_seed'";
                return Agents();
            }
            PRG* prg = new PRG(seed);
            value = [prg](const ValueSpace* valSpace) { return valSpace->rand(prg); };
        } else {
            qWarning() << "[Agent::createAgents()]: unable to parse" << command
                       << "The command should looks like: '*numAgents;[min|max|rand_seed]'";
            return Agents();
        }

        Agents agents;
        for (int agentId = 0; agentId < numAgents; ++agentId) {
            Attributes attrs(modelPlugin->agentAttrSpace().size());
            for (ValueSpace* valSpace : modelPlugin->agentAttrSpace()) {
                attrs.replace(valSpace->id(), valSpace->attrName(), value(valSpace));
            }
            agents.emplace_back(new Agent(agentId, attrs));
        }
        return agents;
    } else if (command.startsWith("#")) {
        cmds.removeFirst();
        if (cmds.size() != modelPlugin->agentAttrSpace().size()) {
            qWarning() << "[Agent::createAgents()]: unable to parse" << command
                       << "The command should looks like: '#numAgents;attrName_[min|max|rand_seed|value_value]'"
                       << "and must contain all attributes of the current model (i.e., '"
                       << modelPlugin->agentAttrNames() << "').";
            return Agents();
        }

        std::vector<Attributes> agentsAttrs;
        agentsAttrs.reserve(numAgents);
        for (int i = 0; i < numAgents; ++i) {
            Attributes attrs(modelPlugin->agentAttrSpace().size());
            agentsAttrs.emplace_back(attrs);
        }

        std::function<Value()> value;
        for (const QString& cmd : cmds) {
            QStringList attrCmd = cmd.split("_");
            const ValueSpace* valSpace = modelPlugin->agentAttrSpace().value(attrCmd.first(), nullptr);
            if (!valSpace) {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The attribute '" << attrCmd.first() << "' does not belong to the model.";
                return Agents();
            } else if (attrCmd.size() == 2) {
                if (attrCmd.at(1) == "min") {
                    value =[valSpace]() { return valSpace->min(); };
                } else if (attrCmd.at(1) == "max") {
                    value =[valSpace]() { return valSpace->max(); };
                } else {
                    qWarning() << "[Agent::createAgents()]: unable to parse" << command
                               << "The command " << attrCmd.at(1) << "should be 'min' or 'max'";
                    return Agents();
                }
            } else if (attrCmd.size() == 3) {
                if (attrCmd.at(1) == "rand") {
                    int seed = attrCmd.at(2).toInt(&ok);
                    if (!ok) {
                        qWarning() << "[Agent::createAgents()]: unable to parse" << command
                                   << "The command should looks like: '#numAgents;attrName_rand_seed;...'";
                        return Agents();
                    }
                    PRG* prg = new PRG(seed);
                    value =[valSpace, prg]() { return valSpace->rand(prg); };
                } else if (attrCmd.at(1) == "value"){
                    QString valStr = attrCmd.at(2);
                    value =[valSpace, valStr]() { return valSpace->validate(valStr); };
                } else {
                    qWarning() << "[Agent::createAgents()]: unable to parse" << command
                               << "The command " << attrCmd.at(1) << "should be 'rand' or 'value'";
                    return Agents();
                }
            } else {
                qWarning() << "[Agent::createAgents()]: unable to parse" << command
                           << "The command " << cmd
                           << "should looks like: attrName_[min|max|rand_seed|value_value]";
                return Agents();
            }

            for (Attributes& attrs : agentsAttrs) {
                attrs.replace(valSpace->id(), valSpace->attrName(), value());
            }
        }

        Agents agents;
        agents.reserve(numAgents);
        for (int agentId = 0; agentId < numAgents; ++agentId) {
            agents.emplace_back(new Agent(agentId, agentsAttrs.at(agentId)));
        }
        return agents;
    }

    qWarning() << "[Agent::createAgents()]: the command"
               << command << "is invalid!";
    return Agents();
}

Agents Agent::readFromFile(const QString &filePath, const ModelPlugin* modelPlugin)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Agent::readFromFile]: unable to read csv file with the set of agents." << filePath;
        return Agents();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd() && modelPlugin) {
        header = in.readLine().split(",");
        foreach (QString attrName, header) {
            if (attrName != "x" && attrName != "y"
                    && !modelPlugin->agentAttrSpace().contains(attrName)) {
                header.clear();
                break;
            }
        }
    }

    if (header.isEmpty()) {
        qWarning() << "[Agent::readFromFile]: unable to read the set of agents from" << filePath
                   << "Expected properties:" << modelPlugin->agentAttrSpace().keys();
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
                ValueSpace* valSpace = modelPlugin->agentAttrSpace().value(header.at(i));
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

Agent::Agent(int id, Attributes attr, int x, int y)
    : m_id(id)
    , m_attrs(attr)
    , m_x(x)
    , m_y(y)
{
}

Agent::Agent(int id, Attributes attr)
    : Agent(id, attr, 0, id)
{
}

}
