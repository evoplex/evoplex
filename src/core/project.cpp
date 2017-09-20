/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QFile>
#include <QVector>
#include <QStringList>
#include <QTextStream>

#include "core/project.h"
#include "utils/utils.h"

Project::Project(MainApp* mainApp, int id, const QString& name, const QString& dir)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_name(name)
    , m_dir(dir)
    , m_lastExpId(-1)
{
    if (m_name.isEmpty()) {
        m_name = QString("Project%1").arg(id);
    }
}

Project::~Project()
{
    qDeleteAll(m_experiments);
}

void Project::runAll()
{
    QHash<int, Experiment*>::iterator it;
    for (it = m_experiments.begin(); it != m_experiments.end(); ++it)
        it.value()->run();
}

const int Project::newExperiment(const QStringList& header, const QStringList& values, QString& errorMsg)
{
    if (header.isEmpty() || values.isEmpty() || header.size() != values.size()) {
        errorMsg = "The 'header' and 'values' cannot be empty and must have the same number of elements.";
        return -1;
    }

    // find the model and graph for this experiment
    const int headerGraphId = header.indexOf(GENERAL_ATTRIBUTE_GRAPHID);
    const int headerModelId = header.indexOf(GENERAL_ATTRIBUTE_MODELID);
    if (headerGraphId < 0 && headerModelId < 0) {
        errorMsg = "The experiment should have both graphId and modelId.";
        return -1;
    }

    // check if the model and graph are available
    const MainApp::GraphPlugin* gPlugin = m_mainApp->getGraph(values.at(headerGraphId));
    const MainApp::ModelPlugin* mPlugin = m_mainApp->getModel(values.at(headerModelId));
    if (!gPlugin || !mPlugin) {
        errorMsg = QString("The graphId (%1) or modelId (%2) are not available."
                           " Make sure to load them before trying to add this experiment.")
                           .arg(values.at(headerGraphId)).arg(values.at(headerModelId));
        return -1;
    }

    // make sure that the chosen graphId is allowed in this model
    if (!mPlugin->supportedGraphs.contains(gPlugin->uid)) {
        QString supportedGraphs = mPlugin->supportedGraphs.toList().join(", ");
        errorMsg = QString("The graphId (%1) cannot be used in this model (%2). The allowed ones are: %3")
                           .arg(gPlugin->uid).arg(mPlugin->uid).arg(supportedGraphs);
        return -1;
    }

    // we assume that all graph/model attributes start with 'uid_'
    const QString& graphId_ = gPlugin->uid + "_";
    const QString& modelId_ = mPlugin->uid + "_";

    // get the value of each attribute and make sure they are valid
    QStringList failedAttributes;
    Attributes* generalAttrs = new Attributes(m_mainApp->getGeneralAttrSpace().size());
    Attributes* modelAttrs = new Attributes(mPlugin->modelAttrSpace.size());
    Attributes* graphAttrs = new Attributes(gPlugin->graphAttrSpace.size());
    for (int i = 0; i < values.size(); ++i) {
        const QString& vStr = values.at(i);
        QString attrName = header.at(i);
        bool ok = false;

        AttributesSpace::const_iterator gps = m_mainApp->getGeneralAttrSpace().find(attrName);
        if (gps != m_mainApp->getGeneralAttrSpace().end()) {
            Value value = Utils::validateParameter(gps.value().second, vStr);
            if (value.isValid()) {
                generalAttrs->replace(gps.value().first, attrName, value);
                ok = true;
            }
        } else {
            QPair<int, QString> attrSpace;
            Attributes* attributes = nullptr;
            if (attrName.startsWith(modelId_)) {
                attrName = attrName.remove(modelId_);
                attrSpace = mPlugin->modelAttrSpace.value(attrName);
                attributes = modelAttrs;
            } else if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                attrSpace = gPlugin->graphAttrSpace.value(attrName);
                attributes = graphAttrs;
            }

            if (attributes && !attrSpace.second.isEmpty()) {
                Value value = Utils::validateParameter(attrSpace.second, vStr);
                if (value.isValid()) {
                    attributes->replace(attrSpace.first, attrName, value);
                    ok = true;
                }
            }
        }

        if (!ok) {
            failedAttributes.append(attrName);
        }
    }

    if (!failedAttributes.isEmpty()) {
        errorMsg = QString("The following attributes are missing or invalid: %1").arg(failedAttributes.join(","));
        delete generalAttrs;
        delete graphAttrs;
        delete modelAttrs;
        return -1;
    }

    // that's great! everything seems to be valid
    ++m_lastExpId;
    m_experiments.insert(m_lastExpId,
        new Experiment(m_mainApp, m_lastExpId, m_id, generalAttrs, modelAttrs, graphAttrs));
    qDebug() << "[Project]: New experiment has been loaded.";
    return m_lastExpId;
}

const int Project::importExperiments(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the experiments." << filePath;
        return -1;
    }

    QTextStream in(&file);

    // read header
    const QStringList header = in.readLine().split(",");
    if (header.isEmpty()) {
        qWarning() << "[FileMgr]: unable to read the experiments from" << filePath
                   << "The header must have:" << m_mainApp->getGeneralAttrSpace();
        return -1;
    }

    // import experiments
    int failures = 0;
    int row = 1;
    while (!in.atEnd()) {
        const QStringList values = in.readLine().split(",");
        QString errorMsg;
        if (newExperiment(header, values, errorMsg) == -1) {
            qWarning() << "[FileMgr]: unable to read the experiment at" << row << filePath
                       << "\nError: \"" << errorMsg;
            ++failures;
        }
        ++row;
    }
    file.close();
    return failures;
}
