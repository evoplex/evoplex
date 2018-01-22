/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFile>
#include <QTextStream>
#include <QtDebug>

#include "agent.h"
#include "modelplugin.h"

namespace evoplex
{

Agents Agent::readFromFile(const QString &filePath, const ModelPlugin* modelPlugin)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the set of agents." << filePath;
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
        qWarning() << "[FileMgr]: unable to read the set of agents from" << filePath
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
            qWarning() << "[FileMgr]: rows must have the same number of columns!";
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
