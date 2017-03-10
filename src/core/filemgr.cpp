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

QVector<AbstractAgent*> FileMgr::importAgents(const QString& filePath, const QString& modelId)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the set of agents." << filePath;
        return QVector<AbstractAgent*>();
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
        return QVector<AbstractAgent*>();
    }

    // create agents
    QVector<AbstractAgent*> agents;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[FileMgr]: rows must have the same number of columns!";
            qDeleteAll(agents);
            return agents;
        }

        QVariantHash parameters;
        for (int i = 0; i < values.size(); ++i) {
            const QString& space = modelPlugin->agentAttrSpace.value(header.at(i));
            QVariant value = Utils::validateParameter(space, values.at(i));
            if (!value.isValid()) {
                qDeleteAll(agents);
                return agents;
            }
            parameters.insert(header.at(i), value);
        }

        agents.append(new AbstractAgent(parameters));
    }
    file.close();

    return agents;
}

int FileMgr::importExperiments(const QString& filePath, int projId)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the experiments." << filePath;
        return -1;
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        foreach (const QString& paramName, m_mainApp->getGeneralAttrSpace()) {
            if (!header.contains(paramName)) {
                header.clear();
                break;
            }
        }
    }

    if (header.isEmpty()) {
        qWarning() << "[FileMgr]: unable to read the experiments from" << filePath
                   << "The header must have:" << m_mainApp->getGeneralAttrSpace();
        return -1;
    }

    // import experiments
    int failures = 0;
    int row = 1;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");

        // Agents: before doing anything else, if a csv file was specified,
        // just make sure that this file really exists. We do not validate the
        // file here because it would affect performace unnecessarily.
        bool validAgents = true;
        const QString& agents = values.at(header.indexOf(GENERAL_ATTRIBUTE_AGENTS));
        if (agents.endsWith(".csv") && !QFile(agents).exists()) {
            qWarning() << "[FileMgr]: unable to import an experiment."
                       << "Unable to find the csv file with the agents.";
            validAgents = false;
        } else {
            // not from a file; ok, just check if it's really a number
            agents.toInt(&validAgents);
        }

        // finally, try to add the experiment
        QString errorMsg;
        if (!validAgents || !m_mainApp->getProject(projId)->newExperiment(header, values, errorMsg)) {
            qWarning() << "Error: \"" << errorMsg << "\n\"Failed at" << row << filePath;
            ++failures;
        }
        ++row;
    }
    file.close();
    return failures;
}
