/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QVector>
#include <QStringList>
#include <QTextStream>

#include "project.h"
#include "experiment.h"
#include "utils.h"

namespace evoplex {

Project::Project(MainApp* mainApp, int id, const QString& name, const QString& dest)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_name(name)
    , m_dest(dest)
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

        AttributesSpace::const_iterator gps = m_mainApp->getGeneralAttrSpace().find(attrName);
        if (gps != m_mainApp->getGeneralAttrSpace().end()) {
            Value value = Utils::validateParameter(gps.value().second, vStr);
            if (value.isValid()) {
                generalAttrs->replace(gps.value().first, attrName, value);
            } else {
                failedAttributes.append(attrName);
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
                } else {
                    failedAttributes.append(attrName);
                }
            }
        }
    }

    if (!failedAttributes.isEmpty()) {
        errorMsg = QString("The following attributes are missing/invalid: %1").arg(failedAttributes.join(","));
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

bool Project::saveProject(const QString& dest, const QString& projectName)
{
    emit (progressSave(0));
    m_dest = dest.isEmpty() ? m_dest : dest;
    m_name = projectName.isEmpty() ? m_name : projectName;

    QDir dir(m_dest);
    if (m_dest.isEmpty() || m_name.isEmpty() || !dir.mkpath(m_name)) {
        qWarning() << "[Project] unable to save project" << m_name
                 << "The destination is invalid!" << m_dest;
        return false;
    }
    dir.cd(m_name);

    emit (progressSave(5));
    QFile experimentsFile(dir.absoluteFilePath(m_name + ".csv"));
    if (!experimentsFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "[Project] unable to save project" << m_name
                   << "Make sure the destination directory is writtable."
                   << dir.absolutePath();
        return false;
    }

    emit (progressSave(10));
    QStringList header;
    QHash<int, Experiment*>::const_iterator it;
    for (it = m_experiments.begin(); it != m_experiments.end(); ++it) {
        header.append(it.value()->getGeneralAttrs()->names().toList());

        // prefix all model attributes with the modelId
        const QString& modelId = it.value()->getGeneralAttrs()->value(GENERAL_ATTRIBUTE_MODELID).toQString();
        const QVector<QString>& modelAttrNames = it.value()->getModelAttrs()->names();
        foreach (const QString& attrName, modelAttrNames) {
            header.append(modelId + "_" + attrName);
        }

        // prefix all graph attributes with the graphId
        const QString& graphId = it.value()->getGeneralAttrs()->value(GENERAL_ATTRIBUTE_GRAPHID).toQString();
        const QVector<QString>& graphAttrNames = it.value()->getGraphAttrs()->names();
        foreach (const QString& attrName, graphAttrNames) {
            header.append(graphId + "_" + attrName);
        } 
    }
    // remove duplicates
    emit (progressSave(25));
    header = header.toSet().toList();

    emit (progressSave(30));
    QTextStream out(&experimentsFile);
    out << header.join(",") + "\n";

    emit (progressSave(35));
    for (it = m_experiments.begin(); it != m_experiments.end(); ++it) {
        const Attributes* generalAttrs = it.value()->getGeneralAttrs();
        const Attributes* modelAttrs = it.value()->getModelAttrs();
        const Attributes* graphAttrs = it.value()->getGraphAttrs();

        const QString& modelId = it.value()->getGeneralAttrs()->value(GENERAL_ATTRIBUTE_MODELID).toQString();
        const QString& graphId = it.value()->getGeneralAttrs()->value(GENERAL_ATTRIBUTE_GRAPHID).toQString();

        QStringList values;
        foreach (QString attrName, header) {
            int idx = generalAttrs->indexOf(attrName);
            if (idx != -1) {
                values.append(generalAttrs->value(idx).toQString());
            } else if (attrName.startsWith(modelId)) {
                idx = modelAttrs->indexOf(attrName.remove(modelId + "_"));
                if (idx != -1) values.append(modelAttrs->value(idx).toQString());
            } else if (attrName.startsWith(graphId)) {
                idx = graphAttrs->indexOf(attrName.remove(graphId + "_"));
                if (idx != -1) values.append(graphAttrs->value(idx).toQString());
            } else {
                values.append(""); // not found; leave empty
            }
        }
        Q_ASSERT(header.size() == values.size());
        out << values.join(",") + "\n";
    }
    experimentsFile.close();
    emit (progressSave(100));
    return true;
}

}
