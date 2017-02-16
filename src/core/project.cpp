/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QVector>
#include <QtDebug>

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

bool Project::newExperiment(const QVariantHash& generalParams,
        const QVariantHash& modelParams, const QVariantHash& graphParams)
{
    ++m_lastExpId;

    m_experiments.insert(m_lastExpId,
        new Experiment(m_mainApp, m_lastExpId, m_id,
                       generalParams, modelParams, graphParams));

    return true;
}

bool Project::newExperiment(const QStringList& header, const QStringList& values)
{
    if (header.size() != values.size()) {
        qWarning() << "[Project]: unable to add a new experiment."
                   << "The header and values must have the same number of elements.";
        return false;
    }

    // find the model and graph for this experiment
    const int headerGraphId = header.indexOf(GENERAL_ATTRIBUTE_GRAPHID);
    const int headerModelId = header.indexOf(GENERAL_ATTRIBUTE_MODELID);
    if (headerGraphId < 0 && headerModelId < 0) {
        qWarning() << "[Project]: unable to add a new experiment."
                   << "The experiment should have both graphId and modelId.";
        return false;
    }

    // check if the model and graph are available
    const MainApp::GraphPlugin* gPlugin = m_mainApp->getGraph(values.at(headerGraphId));
    const MainApp::ModelPlugin* mPlugin = m_mainApp->getModel(values.at(headerModelId));
    if (!gPlugin || !mPlugin) {
        qWarning() << "[Project]: unable to add a new experiment."
                   << "The graphId (" << values.at(headerGraphId) << ") or modelId ("
                   << values.at(headerModelId) << ") are not available in the software."
                   << "Make sure to load them before trying to add this experiment.";
        return false;
    }

    // make sure that the chosen graphId is allowed in this model
    if (!mPlugin->allowedGraphs.contains(gPlugin->uid)) {
        qWarning() << "[Project]: unable to add a new experiment."
                   << "The graphId (" << gPlugin->uid
                   << ") cannot be used in this model (" << mPlugin->uid
                   << ") The allowed ones are:" << mPlugin->allowedGraphs;
        return false;
    }

    // get the value of each parameter and make sure they are valid
    QVariantHash generalParams;
    QVariantHash modelParams;
    QVariantHash graphParams;
    const QHash<QString,QString>& gpSpace = m_mainApp->getGeneralParamsSpace();
    for (int i = 0; i < values.size(); ++i) {
        const QString& name = header.at(i);
        const QString& vStr = values.at(i);

        if (gpSpace.contains(name)) {
            QVariant value = Utils::validateParameter(gpSpace.value(name), vStr);
            if (value.isValid()) generalParams.insert(name, value);
        } else if (mPlugin->modelParamsSpace.contains(name)) {
            QVariant value = Utils::validateParameter(mPlugin->modelParamsSpace.value(name), vStr);
            if (value.isValid()) modelParams.insert(name, value);
        } else if (gPlugin->graphParamsSpace.contains(name)) {
            QVariant value = Utils::validateParameter(gPlugin->graphParamsSpace.value(name), vStr);
            if (value.isValid()) graphParams.insert(name, value);
        }
    }

    if (generalParams.size() != gpSpace.size()
            || modelParams.size() != mPlugin->defaultModelParams.size()
            || graphParams.size() != gPlugin->graphParamsSpace.size())
    {
        qWarning() << "[Project]: unable to add a new experiment."
                   << "The set of parameters is invalid.";
        return false;
    }

    // that's great! everything seems to be valid
    return newExperiment(generalParams, modelParams, graphParams);
}
