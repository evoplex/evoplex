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

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>
#include <functional>

#include "nodesgenerator.h"
#include "modelplugin.h"
#include "utils.h"

namespace evoplex
{

NodesGenerator::NodesGenerator(const AttributesScope& nodeAttrsScope)
    : m_attrsScope(nodeAttrsScope)
{
}

QString NodesGenerator::enumToString(Function func)
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

NodesGenerator::Function NodesGenerator::enumFromString(const QString& funcStr)
{
    if (funcStr == "min") return F_Min;
    else if (funcStr == "max") return F_Max;
    else if (funcStr == "rand") return F_Rand;
    else if (funcStr == "value") return F_Value;
    return F_Invalid;
}

/*********************/

AGFromFile::AGFromFile(const AttributesScope& attrsScope, const QString& filePath)
    : NodesGenerator(attrsScope)
    , m_filePath(filePath)
{
    m_command = filePath;
}

Nodes AGFromFile::create(std::function<void(int)> progress)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "unable to read csv file with the set of nodes." << m_filePath;
        return Nodes();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        bool hasCoordX = false;
        bool hasCoordY = false;
        header = in.readLine().split(",");
        for (int i = 0; i < header.size(); ++i) {
            QString attrName = header.at(i);
            for (int j = i+1; j < header.size(); ++j) {
                if (attrName == header.at(j)) {
                    qWarning() << "unable to read the set of nodes from" << m_filePath
                               << QString("The headers should be unique! '%1' is duplicated.").arg(attrName);
                    return Nodes();
                }
            }

            if (attrName == "x") {
                hasCoordX = true;
            } else if (attrName == "y") {
                hasCoordY = true;
            }
        }

        if (hasCoordX != hasCoordY) {
            qWarning() << "unable to read the set of nodes from" << m_filePath
                       << "One of the 2d coordinates are missing. Make sure you have both 'x' and 'y' columns.";
            return Nodes();
        }
    }

    for (const auto* vs : m_attrsScope) {
        if (!header.contains(vs->attrName())) {
            qWarning() << "the nodes from '" << m_filePath << "' are incompatible with the model."
                       << "Expected attributes:" << m_attrsScope.keys();
            return Nodes();
        }
    }

    // create nodes
    int id = 0;
    Nodes nodes;
    bool isValid = true;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "rows must have the same number of columns!";
            isValid = false;
            break;
        }

        int coordX = 0;
        int coordY = id;
        Attributes attributes(m_attrsScope.size());
        for (int i = 0; i < values.size(); ++i) {
            if (header.at(i) == "x") {
                coordX = values.at(i).toInt(&isValid);
            } else if (header.at(i) == "y") {
                coordY = values.at(i).toInt(&isValid);
            } else {
                AttributeRange* attrRange = m_attrsScope.value(header.at(i));
                if (attrRange) { // is null if the column is not required
                    Value value = attrRange->validate(values.at(i));
                    if (!value.isValid()) {
                        isValid = false;
                        break;
                    }
                    attributes.replace(attrRange->id(), header.at(i), value);
                }
            }
        }
        nodes.emplace_back(new Node(id, attributes, coordX, coordY));
        progress(id);
        ++id;
    }
    file.close();

    if (isValid) {
        nodes.shrink_to_fit();
    } else {
        Utils::deleteAndShrink(nodes);
    }

    return nodes;
}

/*********************/

AGSameFuncForAll::AGSameFuncForAll(const AttributesScope& attrsScope, const int numNodes,
                                   const Function& func, const Value& funcInput)
    : NodesGenerator(attrsScope)
    , m_numNodes(numNodes)
    , m_function(func)
    , m_functionInput(funcInput)
    , m_prg(nullptr)
{
    Q_ASSERT_X(m_numNodes > 0, "AGSameFuncForAll", "number of nodes must be >0");
    Q_ASSERT_X(m_function != F_Invalid, "AGSameFuncForAll", "the function must be valid!");

    switch (m_function) {
    case F_Min:
        m_command = QString("*%1;min").arg(m_numNodes);
        f_value = [](const AttributeRange* attrRange) { return attrRange->min(); };
        break;
    case F_Max:
        m_command = QString("*%1;max").arg(m_numNodes);
        f_value = [](const AttributeRange* attrRange) { return attrRange->max(); };
        break;
    case F_Rand:
        Q_ASSERT_X(funcInput.type() == Value::INT, "AGSameFuncForAll", "rand function expects an integer seed.");
        m_command = QString("*%1;rand_%2").arg(m_numNodes).arg(funcInput.toQString());
        m_prg = new PRG(funcInput.toInt());
        f_value = [this](const AttributeRange* attrRange) { return attrRange->rand(m_prg); };
        break;
    default:
        qFatal("invalid function!");
        break;
    }
}

AGSameFuncForAll::~AGSameFuncForAll()
{
    delete m_prg;
}

Nodes AGSameFuncForAll::create(std::function<void(int)> progress)
{
    Nodes nodes;
    nodes.reserve(m_numNodes);
    for (int nodeId = 0; nodeId < m_numNodes; ++nodeId) {
        Attributes attrs(m_attrsScope.size());
        for (AttributeRange* attrRange : m_attrsScope) {
            attrs.replace(attrRange->id(), attrRange->attrName(), f_value(attrRange));
        }
        nodes.emplace_back(new Node(nodeId, attrs));
        progress(nodeId);
    }
    return nodes;
}

/*********************/

AGDiffFunctions::AGDiffFunctions(const AttributesScope& attrsScope, const int numNodes,
                                 std::vector<AttrCmd> attrCmds)
    : NodesGenerator(attrsScope)
    , m_numNodes(numNodes)
    , m_attrCmds(attrCmds)
{
    Q_ASSERT_X(m_numNodes > 0, "AGDiffFunctions", "number of nodes must be >0");
    m_command = QString("#%1").arg(m_numNodes);
    for (const AttrCmd& cmd : m_attrCmds) {
        m_command += QString(";%1_%2").arg(cmd.attrName).arg(enumToString(cmd.func));
        if (cmd.funcInput.isValid()) {
            m_command += "_" + cmd.funcInput.toQString();
        }
    }
}

Nodes AGDiffFunctions::create(std::function<void(int)> progress)
{
    std::vector<Attributes> nodesAttrs;
    nodesAttrs.reserve(m_numNodes);
    for (int i = 0; i < m_numNodes; ++i) {
        Attributes attrs(m_attrsScope.size());
        nodesAttrs.emplace_back(attrs);
    }

    std::function<Value()> value;
    for (const AttrCmd& cmd : m_attrCmds) {
        const AttributeRange* attrRange = m_attrsScope.value(cmd.attrName, nullptr);
        Q_ASSERT_X(attrRange, "AGDiffFunctions", "unable to find the attribute range");

        PRG* prg = nullptr;
        switch (cmd.func) {
        case F_Min:
            value = [attrRange]() { return attrRange->min(); };
            break;
        case F_Max:
            value = [attrRange]() { return attrRange->max(); };
            break;
        case F_Rand:
            prg = new PRG(cmd.funcInput.toInt());
            value = [attrRange, prg]() { return attrRange->rand(prg); };
            break;
        case F_Value:
            value = [cmd]() { return cmd.funcInput; };
            break;
        default:
            qFatal("invalid function!");
        }

        for (Attributes& attrs : nodesAttrs) {
            attrs.replace(attrRange->id(), attrRange->attrName(), value());
        }
        delete prg;
    }

    Nodes nodes;
    nodes.reserve(m_numNodes);
    for (int nodeId = 0; nodeId < m_numNodes; ++nodeId) {
        nodes.emplace_back(new Node(nodeId, nodesAttrs.at(nodeId)));
        progress(nodeId);
    }
    return nodes;
}

/*********************/

bool NodesGenerator::saveToFile(QString& filePath, Nodes nodes, std::function<void(int)>& progress)
{
    if (nodes.empty()) {
        qWarning() << "tried to save an empty set of nodes.";
        return false;
    }

    if (!filePath.endsWith(".csv")) {
        filePath += ".csv";
    }

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "failed to save the set of nodes to file." << filePath;
        return false;
    }

    QTextStream out(&file);
    const std::vector<QString>& header = nodes.front()->attrs().names();
    for (const QString& col : header) {
        out << col << ",";
    }
    out << "x,y\n";

    for (Node* node : nodes) {
        for (const Value& value : node->attrs().values()) {
            out << value.toQString() << ",";
        }
        out << node->x() << ",";
        out << node->y() << "\n";

        out.flush();
        progress(node->id());
    }

    file.close();
    return true;
}

NodesGenerator* NodesGenerator::parse(const AttributesScope& nodeAttrsScope,
                                      const QString& command, QString& errMsg)
{
    NodesGenerator* ag = nullptr;

    if (QFileInfo::exists(command)) {
        ag = new AGFromFile(nodeAttrsScope, command);
    } else {
        QStringList cmds = command.split(";");
        if (cmds.size() < 2) {
            errMsg = QString("The command %1 is invalid!").arg(command);
            qWarning() << errMsg;
            return nullptr;
        }

        bool ok;
        QString numNodesStr = cmds.at(0);
        const int numNodes = numNodesStr.remove(0,1).toInt(&ok);
        if (!ok) {
            errMsg = QString("Unable to parse '%1'."
                    "\n'%2' should be an integer representing the number of nodes.")
                    .arg(command).arg(numNodesStr);
            qWarning() << errMsg;
            return nullptr;
        }

        if (command.startsWith("*")) {
            if (cmds.size() != 2) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '*numNodes;[min|max|rand_seed]'")
                        .arg(command).arg(numNodesStr);
                qWarning() << errMsg;
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
                            "It should look like: '*numNodes;rand_seed'")
                            .arg(command);
                    qWarning() << errMsg;
                    return nullptr;
                }
            } else if (func == F_Invalid) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '*numNodes;[min|max|rand_seed]'")
                        .arg(command);
                qWarning() << errMsg;
                return nullptr;
            }
            ag = new AGSameFuncForAll(nodeAttrsScope, numNodes, func, value);
        } else if (command.startsWith("#")) {
            cmds.removeFirst();
            if (cmds.size() != nodeAttrsScope.size()) {
                errMsg = QString("Unable to parse '%1'."
                        "It should look like: '#numNodes;attrName_[min|max|rand_seed|value_value]'"
                        "and must contain all attributes of the current model (i.e., '%2')")
                        .arg(command).arg(nodeAttrsScope.keys().join(", "));
                qWarning() << errMsg;
                return nullptr;
            }

            std::vector<AGDiffFunctions::AttrCmd> attrCmds;
            attrCmds.reserve(cmds.size());

            for (const QString& cmd : cmds) {
                AGDiffFunctions::AttrCmd attrCmd;
                QStringList attrCmdStr = cmd.split("_");

                attrCmd.attrName = attrCmdStr.at(0);
                const AttributeRange* attrRange = nodeAttrsScope.value(attrCmd.attrName, nullptr);
                if (attrRange) {
                    attrCmd.attrId = attrRange->id();
                } else {
                    errMsg = QString("Unable to parse '%1'."
                            "The attribute '%2' does not belong to the current model.")
                            .arg(command).arg(attrCmd.attrName);
                    qWarning() << errMsg;
                    return nullptr;
                }

                attrCmd.func = enumFromString(attrCmdStr.at(1));
                if (attrCmd.func == F_Invalid) {
                    errMsg = QString("Unable to parse '%1'."
                                "The function '%2' is invalid.")
                                .arg(command).arg(attrCmdStr.at(1));
                    qWarning() << errMsg;
                    return nullptr;
                } else if (attrCmd.func == F_Rand) {
                    attrCmd.funcInput = Value(attrCmdStr.at(2).toInt(&ok)); // seed
                    if (!ok) {
                        errMsg = QString("Unable to parse '%1'. The PRG seed should be an integer!").arg(command);
                        qWarning() << errMsg;
                        return nullptr;
                    }
                } else if (attrCmd.func == F_Value){
                    attrCmd.funcInput = nodeAttrsScope.value(attrCmd.attrName)->validate(attrCmdStr.at(2));
                    if (!attrCmd.funcInput.isValid()) {
                        errMsg = QString("Unable to parse '%1'. The value is invalid!").arg(command);
                        qWarning() << errMsg;
                        return nullptr;
                    }
                }

                attrCmds.emplace_back(attrCmd);
            }

            ag = new AGDiffFunctions(nodeAttrsScope, numNodes, attrCmds);
        }
    }

    if (!ag) {
        errMsg = QString("the command '%1'. is invalid!").arg(command);
        qWarning() << errMsg;
        return nullptr;
    }

    Q_ASSERT_X(command == ag->command(), "NodesGenerator", "something went extremely wrong with the command string!");
    return ag;
}


} // evoplex
