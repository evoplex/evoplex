/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QtDebug>

#include "core/filemgr.h"
#include "core/project.h"
#include "utils/utils.h"

FileMgr::FileMgr(MainApp* mainApp): m_mainApp(mainApp)
{
}

Agents* FileMgr::importAgents(const QString& filePath, const QString& modelId) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the set of agents." << filePath;
        return nullptr;
    }

    QTextStream in(&file);
    const MainApp::ModelPlugin* modelPlugin = m_mainApp->getModel(modelId);

    // read and validate header
    QStringList header;
    if (!in.atEnd() && !modelPlugin) {
        header = in.readLine().split(",");
        if (header.size() == modelPlugin->agentAttrSpace.size()) {
            foreach (QString paramName, header) {
                if (!modelPlugin->agentAttrSpace.contains(paramName)) {
                    header.clear();
                    break;
                }
            }
        } else {
            header.clear();
        }
    }

    if (header.isEmpty()) {
        qWarning() << "[FileMgr]: unable to read the set of agents from" << filePath
                   << "Expected properties:" << modelPlugin->agentAttrSpace.keys();
        return nullptr;
    }

    // create agents
    bool isValid = true;
    Agents* agents = new Agents();
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[FileMgr]: rows must have the same number of columns!";
            isValid = false;
            break;
        }

        QVariantHash attributes;
        for (int i = 0; i < values.size(); ++i) {
            const QString& space = modelPlugin->agentAttrSpace.value(header.at(i));
            QVariant value = Utils::validateParameter(space, values.at(i));
            if (!value.isValid()) {
                isValid = false;
                break;
            }
            attributes.insert(header.at(i), value);
        }
        agents->push_back(new AbstractAgent(attributes));
    }
    file.close();

    if (isValid) {
        agents->squeeze();
    } else {
        Utils::deleteQVectorP(agents);
    }

    return agents;
}
