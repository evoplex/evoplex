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

#include <functional>
#include <memory>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>

#include "attrsgenerator.h"
#include "modelplugin.h"
#include "utils.h"

namespace evoplex {

QString AttrsGenerator::enumToString(Function func)
{
    switch (func) {
    case F_Min: return "min";
    case F_Max: return "max";
    case F_Rand: return "rand";
    case F_Value: return "value";
    default: return "invalid";
    }
}

AttrsGenerator::Function AttrsGenerator::enumFromString(const QString& funcStr)
{
    if (funcStr == "min") return F_Min;
    else if (funcStr == "max") return F_Max;
    else if (funcStr == "rand") return F_Rand;
    else if (funcStr == "value") return F_Value;
    return F_Invalid;
}

AttrsGenerator* AttrsGenerator::parse(const AttributesScope& attrsScope,
                                      const QString& cmd, QString& error)
{
    QStringList cmds = cmd.split(";");
    if (cmds.size() < 2) {
        error = QString("The command %1 is invalid!").arg(cmd);
        qWarning() << error;
        return nullptr;
    }

    QString numCopiesStr = cmds.takeFirst();
    bool ok = false;
    const int numCopies = numCopiesStr.remove(0,1).toInt(&ok);
    if (!ok) {
        error = QString("Unable to parse '%1'.\n"
                      "'%2' should be an integer representing the size of the attributes set.")
                      .arg(cmd).arg(numCopiesStr);
        qWarning() << error;
        return nullptr;
    }

    AttrsGenerator* ag = nullptr;
    if (cmd.startsWith("*")) {
        ag = parseStarCmd(attrsScope, numCopies, cmds, error);
    } else if (cmd.startsWith("#")) {
        ag = parseHashCmd(attrsScope, numCopies, cmds, error);
    } else {
        error = QString("the command '%1'. is invalid!").arg(cmd);
    }

    if (!ag) {
        qWarning() << error;
        return nullptr;
    }

    Q_ASSERT_X(cmd == ag->command(), "AttrsGenerator",
               "something went extremely wrong with the command string!");
    return ag;
}

AGSameFuncForAll* AttrsGenerator::parseStarCmd(const AttributesScope& attrsScope,
        const int numCopies, const QStringList& cmds, QString& error)
{
    if (cmds.size() != 1) {
        error = QString("Unable to parse '*%1;%2'."
                      "It should look like: '*integer;[min|max|rand_seed]'")
                      .arg(numCopies).arg(cmds.first());
        qWarning() << error;
        return nullptr;
    }

    const QString& cmd = cmds.first();
    Function func = enumFromString(cmd);
    Value value;

    if (cmd.startsWith("rand_")) {
        func = F_Rand;
        QString seedStr = cmd;

        bool ok = false;
        value = Value(seedStr.remove("rand_").toInt(&ok)); // seed
        if (!ok) {
            error = QString("Unable to parse '*%1;%2'."
                    "It should look like: '*integer;rand_seed'")
                    .arg(numCopies).arg(cmds.first());
            qWarning() << error;
            return nullptr;
        }
    } else if (func == F_Invalid) {
        error = QString("Unable to parse '*%1;%2'."
                "It should look like: '*integer;[min|max|rand_seed]'")
                .arg(numCopies).arg(cmds.first());
        qWarning() << error;
        return nullptr;
    }

    return new AGSameFuncForAll(attrsScope, numCopies, func, value);
}

AGDiffFunctions* AttrsGenerator::parseHashCmd(const AttributesScope& attrsScope,
        const int numCopies, const QStringList& cmds, QString& error)
{
    if (cmds.size() != attrsScope.size()) {
        error = QString("Unable to parse '#%1;%2'."
                "It should look like: '#integer;attrName_[min|max|rand_seed|value_value]'"
                "and must contain all attributes of the current model (i.e., '%3')")
                .arg(numCopies).arg(cmds.join(";")).arg(attrsScope.keys().join(", "));
        qWarning() << error;
        return nullptr;
    }

    std::vector<AGDiffFunctions::AttrCmd> attrCmds;
    attrCmds.reserve(static_cast<size_t>(cmds.size()));

    for (const QString& cmd : cmds) {
        AGDiffFunctions::AttrCmd attrCmd;
        QStringList attrCmdStr = cmd.split("_");

        attrCmd.attrName = attrCmdStr.at(0);
        const AttributeRange* attrRange = attrsScope.value(attrCmd.attrName, nullptr);
        if (attrRange) {
            attrCmd.attrId = attrRange->id();
        } else {
            error = QString("Unable to parse '#%1;%2'.\n"
                    "The attribute '%3' does not belong to the current model.")
                    .arg(numCopies).arg(cmds.join(";")).arg(attrCmd.attrName);
            qWarning() << error;
            return nullptr;
        }

        attrCmd.func = enumFromString(attrCmdStr.at(1));
        if (attrCmd.func == F_Invalid) {
            error = QString("Unable to parse '#%1;%2'.\n The function '%2' is invalid.")
                        .arg(numCopies).arg(cmds.join(";")).arg(attrCmdStr.at(1));
            qWarning() << error;
            return nullptr;
        } else if (attrCmd.func == F_Rand) {
            bool ok = false;
            attrCmd.funcInput = Value(attrCmdStr.at(2).toInt(&ok)); // seed
            if (!ok) {
                error = QString("Unable to parse '#%1;%2'.. The PRG seed should be an integer!")
                            .arg(numCopies).arg(cmds.join(";"));
                qWarning() << error;
                return nullptr;
            }
        } else if (attrCmd.func == F_Value){
            attrCmd.funcInput = attrsScope.value(attrCmd.attrName)->validate(attrCmdStr.at(2));
            if (!attrCmd.funcInput.isValid()) {
                error = QString("Unable to parse '#%1;%2'. The value is invalid!")
                            .arg(numCopies).arg(cmds.join(";"));
                qWarning() << error;
                return nullptr;
            }
        }

        attrCmds.emplace_back(attrCmd);
    }

    return new AGDiffFunctions(attrsScope, numCopies, attrCmds);
}

/****************************************************/
/****************************************************/

AttrsGenerator::AttrsGenerator(const AttributesScope& attrsScope, const int numCopies)
    : m_attrsScope(attrsScope),
      m_numCopies(numCopies)
{
    Q_ASSERT_X(m_numCopies > 0, "AttrsGenerator", "number of copies must be >0");
}

AttrsGenerator::~AttrsGenerator()
{
}

/****************************************************/
/****************************************************/

AGSameFuncForAll::AGSameFuncForAll(const AttributesScope& attrsScope, const int numCopies,
                                   const Function& func, const Value& funcInput)
    : AttrsGenerator(attrsScope, numCopies),
      m_function(func),
      m_functionInput(funcInput),
      m_prg(nullptr)
{
    Q_ASSERT_X(m_function != F_Invalid, "AGSameFuncForAll", "the function must be valid!");

    switch (m_function) {
    case F_Min:
        m_command = QString("*%1;min").arg(m_numCopies);
        f_value = [](const AttributeRange* attrRange) { return attrRange->min(); };
        break;
    case F_Max:
        m_command = QString("*%1;max").arg(m_numCopies);
        f_value = [](const AttributeRange* attrRange) { return attrRange->max(); };
        break;
    case F_Rand:
        Q_ASSERT_X(funcInput.type() == Value::INT, "AGSameFuncForAll", "rand function expects an integer seed.");
        m_command = QString("*%1;rand_%2").arg(m_numCopies).arg(funcInput.toQString());
        m_prg = new PRG(funcInput.toInt());
        f_value = [this](const AttributeRange* attrRange) { return attrRange->rand(m_prg); };
        break;
    default:
        qFatal("invalid function!");
    }
}

AGSameFuncForAll::~AGSameFuncForAll()
{
    delete m_prg;
}

SetOfAttributes AGSameFuncForAll::create(std::function<void(int)> progress)
{
    SetOfAttributes ret;
    ret.reserve(static_cast<size_t>(m_numCopies));
    for (int id = 0; id < m_numCopies; ++id) {
        Attributes attrs(m_attrsScope.size());
        for (AttributeRange* attrRange : m_attrsScope) {
            attrs.replace(attrRange->id(), attrRange->attrName(), f_value(attrRange));
        }
        ret.emplace_back(attrs);
        progress(id);
    }
    return ret;
}

/****************************************************/
/****************************************************/

AGDiffFunctions::AGDiffFunctions(const AttributesScope& attrsScope, const int numCopies,
                                 std::vector<AttrCmd> attrCmds)
    : AttrsGenerator(attrsScope, numCopies),
      m_attrCmds(attrCmds)
{
    m_command = QString("#%1").arg(m_numCopies);
    for (const AttrCmd& cmd : m_attrCmds) {
        m_command += QString(";%1_%2").arg(cmd.attrName).arg(enumToString(cmd.func));
        if (cmd.funcInput.isValid()) {
            m_command += "_" + cmd.funcInput.toQString();
        }
    }
}

SetOfAttributes AGDiffFunctions::create(std::function<void(int)> progress)
{
    SetOfAttributes setOfAttrs;
    setOfAttrs.reserve(static_cast<size_t>(m_numCopies));
    for (int i = 0; i < m_numCopies; ++i) {
        Attributes attrs(m_attrsScope.size());
        setOfAttrs.emplace_back(attrs);
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

        for (Attributes& attrs : setOfAttrs) {
            attrs.replace(attrRange->id(), attrRange->attrName(), value());
        }
        delete prg;
    }

    SetOfAttributes ret;
    ret.reserve(static_cast<size_t>(m_numCopies));
    for (int id = 0; id < m_numCopies; ++id) {
        ret.emplace_back(setOfAttrs.at(id));
        progress(id);
    }
    return ret;
}

} // evoplex
