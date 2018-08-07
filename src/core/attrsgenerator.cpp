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

AttrsGeneratorPtr AttrsGenerator::parse(const AttributesScope& attrsScope,
                                        const QString& cmd, QString& error)
{
    if (cmd.isEmpty()) {
        error = "The command cannot be empty!";
        qWarning() << error;
        return nullptr;
    }

    QStringList cmds = cmd.split(";");
    QString sizeStr = cmds.takeFirst();
    QString _cmd;
    int size = -1;

    bool sizeIsValid = false;
    if (cmds.empty()) { // '*int;min' alias
        size = sizeStr.toInt(&sizeIsValid);
        _cmd = QString("*%1;min").arg(size);
        cmds.push_back("min");
    } else {
        size = sizeStr.remove(0,1).toInt(&sizeIsValid);
        _cmd = cmd;
    }

    if (!sizeIsValid || size < 1) {
        error = "Unable to parse '" + cmd + "'.\nIt should start with a positive"
                " integer representing the size of the attributes set.";
        qWarning() << error;
        return nullptr;
    }

    std::unique_ptr<AttrsGenerator> ag;
    if (_cmd.startsWith("*")) {
        ag = parseStarCmd(attrsScope, size, cmds, error);
    } else if (_cmd.startsWith("#") && !attrsScope.empty()) {
        ag = parseHashCmd(attrsScope, size, cmds, error);
    } else {
        error = "the command '" + cmd + "'. is invalid!";
    }

    if (!ag) {
        qWarning() << error;
        return nullptr;
    }

    Q_ASSERT_X(_cmd == ag->command(), "AttrsGenerator",
               "something went extremely wrong with the command string!");
    return ag;
}

std::unique_ptr<AGSameFuncForAll> AttrsGenerator::parseStarCmd(
        const AttributesScope& attrsScope, const int size,
        const QStringList& cmds, QString& error)
{
    if (cmds.size() != 1) {
        error = QString("Unable to parse '*%1;%2'."
                      "It should look like: '*integer;[min|max|rand_seed]'")
                      .arg(size).arg(cmds.first());
        qWarning() << error;
        return nullptr;
    }

    const QString& cmd = cmds.first();
    Function func = _enumFromString<Function>(cmd);
    Value value;

    if (cmd.startsWith("rand_")) {
        func = Function::Rand;
        value = parseRandSeed(cmd);
        if (!value.isValid()) {
            error = QString("Unable to parse '*%1;%2'."
                    "It should look like: '*%1;rand_seed', where seed is a positive integer!")
                    .arg(size).arg(cmds.first());
            qWarning() << error;
            return nullptr;
        }
    } else if (func == Function::Invalid) {
        error = QString("Unable to parse '*%1;%2'."
                "It should look like: '*integer;[min|max|rand_seed]'")
                .arg(size).arg(cmds.first());
        qWarning() << error;
        return nullptr;
    }

    return std::unique_ptr<AGSameFuncForAll>(
            new AGSameFuncForAll(attrsScope, size, func, value));
}

std::unique_ptr<AGDiffFunctions> AttrsGenerator::parseHashCmd(
        const AttributesScope& attrsScope, const int size,
        const QStringList& cmds, QString& error)
{
    if (cmds.size() != attrsScope.size()) {
        const QStringList expectedAttrs = attrsScope.keys();
        error = QString("Unable to parse '#%1;%2'."
                "It should look like: '#integer;attrName_[min|max|rand_seed|value_value]'"
                "and must contain all attributes of the current model (i.e., '%3')")
                .arg(size).arg(cmds.join(";"), expectedAttrs.join(", "));
        qWarning() << error;
        return nullptr;
    }

    std::vector<AGDiffFunctions::AttrCmd> attrCmds;
    attrCmds.reserve(static_cast<size_t>(cmds.size()));

    for (const QString& cmd : cmds) {
        AGDiffFunctions::AttrCmd attrCmd;
        QStringList attrCmdStr = cmd.split("_");

        attrCmd.attrName = attrCmdStr.at(0);
        auto attrRange = attrsScope.value(attrCmd.attrName, nullptr);
        if (attrRange) {
            attrCmd.attrId = attrRange->id();
        } else {
            error = QString("Unable to parse '#%1;%2'.\n"
                    "The attribute '%3' does not belong to the current model.")
                    .arg(size).arg(cmds.join(";"), attrCmd.attrName);
            qWarning() << error;
            return nullptr;
        }

        attrCmd.func = _enumFromString<Function>(attrCmdStr.at(1));
        if (attrCmd.func == Function::Invalid) {
            error = QString("Unable to parse '#%1;%2'.\n The function '%2' is invalid.")
                        .arg(size).arg(cmds.join(";"), attrCmdStr.at(1));
            qWarning() << error;
            return nullptr;
        } else if (attrCmd.func == Function::Rand) {
            attrCmd.funcInput = parseRandSeed(attrCmdStr.at(2));
            if (!attrCmd.funcInput.isValid()) {
                error = QString("Unable to parse '#%1;%2'.. The PRG seed should be a positive integer!")
                            .arg(size).arg(cmds.join(";"));
                qWarning() << error;
                return nullptr;
            }
        } else if (attrCmd.func == Function::Value){
            attrCmd.funcInput = attrsScope.value(attrCmd.attrName)->validate(attrCmdStr.at(2));
            if (!attrCmd.funcInput.isValid()) {
                error = QString("Unable to parse '#%1;%2'. The value is invalid!")
                            .arg(size).arg(cmds.join(";"));
                qWarning() << error;
                return nullptr;
            }
        }

        attrCmds.emplace_back(attrCmd);
    }

    return std::unique_ptr<AGDiffFunctions>(
            new AGDiffFunctions(attrsScope, size, attrCmds));
}

Value AttrsGenerator::parseRandSeed(QString seedStr)
{
    bool ok = false;
    int seed = seedStr.remove("rand_").toInt(&ok);
    // also avoid things like '-0' and '01'
    if (!ok || seed < 0 || seedStr != QString::number(seed)) {
        return Value();
    }
    return Value(seed);
}

/****************************************************/
/****************************************************/

AttrsGenerator::AttrsGenerator(const AttributesScope& attrsScope, const int size)
    : m_attrsScope(attrsScope),
      m_size(size)
{
    Q_ASSERT_X(m_size > 0, "AttrsGenerator", "number of copies must be >0");
}

/****************************************************/
/****************************************************/

AGSameFuncForAll::AGSameFuncForAll(const AttributesScope& attrsScope, const int size,
                                   const Function& func, const Value& funcInput)
    : AttrsGenerator(attrsScope, size),
      m_function(func),
      m_functionInput(funcInput),
      m_prg(nullptr)
{
    Q_ASSERT_X(m_function != Function::Invalid, "AGSameFuncForAll", "the function must be valid!");

    switch (m_function) {
    case Function::Min:
        m_command = QString("*%1;min").arg(m_size);
        f_value = [](AttributeRangePtr attrRange) { return attrRange->min(); };
        break;
    case Function::Max:
        m_command = QString("*%1;max").arg(m_size);
        f_value = [](AttributeRangePtr attrRange) { return attrRange->max(); };
        break;
    case Function::Rand:
        Q_ASSERT_X(funcInput.type() == Value::INT, "AGSameFuncForAll", "rand function expects an integer seed.");
        m_command = QString("*%1;rand_%2").arg(m_size).arg(funcInput.toQString());
        m_prg = new PRG(funcInput.toUInt());
        f_value = [this](AttributeRangePtr attrRange) { return attrRange->rand(m_prg); };
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
    ret.reserve(static_cast<size_t>(m_size));
    for (int id = 0; id < m_size; ++id) {
        Attributes attrs(m_attrsScope.size());
        for (auto attrRange : m_attrsScope) {
            attrs.replace(attrRange->id(), attrRange->attrName(), f_value(attrRange));
        }
        ret.emplace_back(attrs);
        progress(id);
    }
    return ret;
}

/****************************************************/
/****************************************************/

AGDiffFunctions::AGDiffFunctions(const AttributesScope& attrsScope, const int size,
                                 const std::vector<AttrCmd>& attrCmds)
    : AttrsGenerator(attrsScope, size),
      m_attrCmds(attrCmds)
{
    m_command = QString("#%1").arg(m_size);
    for (const AttrCmd& cmd : m_attrCmds) {
        m_command += QString(";%1_%2").arg(cmd.attrName, _enumToString<Function>(cmd.func));
        if (cmd.funcInput.isValid()) {
            m_command += "_" + cmd.funcInput.toQString();
        }
    }
}

SetOfAttributes AGDiffFunctions::create(std::function<void(int)> progress)
{
    SetOfAttributes setOfAttrs;
    setOfAttrs.reserve(static_cast<size_t>(m_size));
    for (int i = 0; i < m_size; ++i) {
        Attributes attrs(m_attrsScope.size());
        setOfAttrs.emplace_back(attrs);
    }

    std::function<Value()> value;
    for (const AttrCmd& cmd : m_attrCmds) {
        auto attrRange = m_attrsScope.value(cmd.attrName, nullptr);
        Q_ASSERT_X(attrRange, "AGDiffFunctions", "unable to find the attribute range");

        PRG* prg = nullptr;
        switch (cmd.func) {
        case Function::Min:
            value = [attrRange]() { return attrRange->min(); };
            break;
        case Function::Max:
            value = [attrRange]() { return attrRange->max(); };
            break;
        case Function::Rand:
            prg = new PRG(cmd.funcInput.toUInt());
            value = [attrRange, prg]() { return attrRange->rand(prg); };
            break;
        case Function::Value:
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
    ret.reserve(static_cast<size_t>(m_size));
    for (int id = 0; id < m_size; ++id) {
        ret.emplace_back(setOfAttrs.at(static_cast<size_t>(id)));
        progress(id);
    }
    return ret;
}

} // evoplex
