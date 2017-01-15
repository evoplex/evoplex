/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "core/experiment.h"
#include "core/abstractagent.h"
#include "core/filemgr.h"
#include "utils/constants.h"
#include "utils/utils.h"

#include <QtDebug>

Experiment::Experiment(MainApp* mainApp, int id, int projId, const QVariantHash& generalParams,
                       const QVariantHash& modelParams, const QVariantHash& graphParams)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_projId(projId)
    , m_generalParams(generalParams)
    , m_modelParams(modelParams)
    , m_graphParams(graphParams)
{
}

bool Experiment::createTrials()
{
    if (!m_trialsIds.isEmpty()) {
        qWarning() << "[Experiment]: it seems that the trials for this experiment have already been created.";
        return false;
    }

    int seed = m_generalParams.value(GENERAL_PARAMETER_SEED).toInt();
    PRG* prg = new PRG(seed);

    // find out the model and graph
    const QString& modelId = m_generalParams.value(GENERAL_PARAMETER_MODELID).toString();
    const MainApp::ModelPlugin* modelPlugin = m_mainApp->getModel(modelId);
    const QString& graphId = m_generalParams.value(GENERAL_PARAMETER_GRAPHID).toString();
    const MainApp::GraphPlugin* graphPlugin = m_mainApp->getGraph(graphId);

    // create the set of agents
    QVector<AbstractAgent*> agents;
    const QString& agentsStr = m_generalParams.value(GENERAL_PARAMETER_AGENTS).toString();
    if (agentsStr.endsWith(".csv")) {
        // I/O operations are expensive. So, if this is the case, lets do it
        // only once and feed the trials with copies of the population.
        agents = m_mainApp->getFileMgr()->importAgents(agentsStr, modelId);
    } else { // random parameters
        const int agentsSize = agentsStr.toInt();
        agents.reserve(agentsSize);
        for (int i = 0; i < agentsSize; ++i) {
            agents.append(new AbstractAgent(Utils::randomParams(modelPlugin->agentParamsSpace, prg)));
        }
    }

    if (agents.isEmpty()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The set of agents could not be created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete prg;
        qDeleteAll(agents);
        return false;
    }

    const int trials = m_generalParams.value(GENERAL_PARAMETER_TRIALS).toInt();
    m_trialsIds.reserve(trials);

    // create the graph structure
    AbstractGraph* graphObj = graphPlugin->factory->create();
    if (!graphObj || !graphObj->init(agents, m_graphParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete prg;
        delete graphObj;
        qDeleteAll(agents);
        return false;
    }

    // create the model object
    AbstractModel* modelObj = modelPlugin->factory->create();
    modelObj->setup(prg, graphObj); // make the PRG and the graph available in the model
    if (!modelObj || !modelObj->init(m_modelParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete prg;
        delete graphObj;
        delete modelObj;
        qDeleteAll(agents);
        return false;
    }

    // finally, create the first trial
    const int stopAt = m_generalParams.value(GENERAL_PARAMETER_STOPAT).toInt();
    m_trialsIds.append(m_mainApp->getTrialsMgr()->newTrial(m_id, m_projId, modelObj, stopAt));

    // more trials to be created? (ie, trials>1)
    // if all went well for the first trial, we don't need to check everything again
    for (int t = 1; t < trials; ++t) {
        // copy agents
        QVector<AbstractAgent*> agentsCopy;
        agentsCopy.reserve(agents.size());
        for (int a = 1; a < agents.size(); ++a) {
            agentsCopy.append(agents.at(a)->clone());
        }
        // new graph
        graphObj = graphPlugin->factory->create();
        graphObj->init(agentsCopy, m_graphParams);
        // new seed
        ++seed;
        // new model
        modelObj = modelPlugin->factory->create();
        modelObj->setup(new PRG(seed), graphObj);
        modelObj->init(m_modelParams);
        // add trial
        m_trialsIds.append(m_mainApp->getTrialsMgr()->newTrial(m_id, m_projId, modelObj, stopAt));
    }

    return true;
}
