/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/abstractagent.h"
#include "core/experiment.h"
#include "core/experimentsmgr.h"
#include "core/filemgr.h"
#include "utils/constants.h"
#include "utils/utils.h"

Experiment::Experiment(MainApp* mainApp, int id, int projId, const QVariantHash& generalParams,
                       const QVariantHash& modelParams, const QVariantHash& graphParams)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_projId(projId)
    , m_generalParams(generalParams)
    , m_modelParams(modelParams)
    , m_graphParams(graphParams)
    , m_graphPlugin(m_mainApp->getGraph(generalParams.value(GENERAL_ATTRIBUTE_GRAPHID).toString()))
    , m_modelPlugin(m_mainApp->getModel(generalParams.value(GENERAL_ATTRIBUTE_MODELID).toString()))
    , m_numTrials(generalParams.value(GENERAL_ATTRIBUTE_TRIALS).toInt())
{
    m_trials.reserve(m_numTrials);
    m_curSeed = m_generalParams.value(GENERAL_ATTRIBUTE_SEED).toInt();
    m_stopAt = m_generalParams.value(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    setStatus(READY);
}

void Experiment::run()
{
    if (m_expStatus != READY) {
        return;
    }
    setStatus(RUNNING);
    m_mainApp->getExperimentsMgr()->run(this);
}

void Experiment::finished()
{
    m_pauseAt = m_stopAt; // reset the pauseAt flag to maximum
    setStatus(FINISHED);
    foreach (Trial trial, m_trials) {
        if (trial.status != FINISHED) {
            setStatus(READY);
            break;
        }
    }
    m_mainApp->getExperimentsMgr()->finished(this);
}

void Experiment::processTrial(int& trialId)
{
    if (m_expStatus == INVALID) {
        return;
    } else if (!m_trials.contains(trialId)) {
        trialId = createTrial();
        if (trialId == -1) {
            setStatus(INVALID);
            pause();
            return;
        }
    }

    Trial trial = m_trials.value(trialId);
    if (trial.status != READY) {
        return;
    }

    trial.status = RUNNING;

    bool algorithmConverged = false;
    while (trial.currentStep <= m_pauseAt && !algorithmConverged) {
        algorithmConverged = trial.modelObj->algorithmStep();
        ++trial.currentStep;
    }

    if (trial.currentStep >= m_stopAt || algorithmConverged) {
        // TODO: IO stuff
        trial.status = FINISHED;
    } else {
        trial.status = READY;
    }
}

int Experiment::createTrial()
{
    if (m_trials.size() == m_numTrials) {
        qWarning() << "[Experiment]: all the trials for this experiment have already been created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        return -1;
    }

    m_mutex.lock();
    QVector<AbstractAgent> agents = createAgents();
    m_mutex.unlock();
    if (agents.isEmpty()) {
        agents.squeeze();
        return -1;
    }

    AbstractGraph* graphObj = m_graphPlugin->factory->create();
    if (!graphObj || !graphObj->init(agents, m_graphParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        agents.clear();
        agents.squeeze();
        return -1;
    }

    AbstractModel* modelObj = m_modelPlugin->factory->create();
    modelObj->setup(m_curSeed, graphObj); // make the PRG and the graph available in the model
    if (!modelObj || !modelObj->init(m_modelParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        delete modelObj;
        agents.clear();
        agents.squeeze();
        return -1;
    }

    Trial trial;
    trial.modelObj = modelObj;
    trial.status = READY;

    m_mutex.lock();
    int trialId = m_trials.size() - 1;
    m_trials.insert(trialId, trial);
    ++m_curSeed;
    m_mutex.unlock();

    return trialId;
}

QVector<AbstractAgent> Experiment::createAgents()
{
    if (!m_clonableAgents.isEmpty()) {
        return cloneAgents(m_clonableAgents);
    }

    Q_ASSERT(m_trials.size() == 0);

    QVector<AbstractAgent> agents;
    bool isInt;
    int numAgents = m_generalParams.value(GENERAL_ATTRIBUTE_AGENTS).toInt(&isInt);
    if (isInt) { // create a population of agents with random properties?
        agents.reserve(numAgents);
        PRG* prg = new PRG(m_curSeed);
        for (int i = 0; i < numAgents; ++i) {
            agents.push_back(AbstractAgent(Utils::randomParams(m_modelPlugin->agentAttrSpace, prg)));
        }
        delete prg;
    } else { // read population from a text file?
        agents = m_mainApp->getFileMgr()->importAgents(
                    m_generalParams.value(GENERAL_ATTRIBUTE_AGENTS).toString(),
                    m_modelPlugin->uid);
    }
    agents.squeeze();

    if (agents.isEmpty()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The set of agents could not be created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
    } else {
        if (m_numTrials > 1) {
            m_clonableAgents = cloneAgents(agents);
        }
    }
    return agents;
}

QVector<AbstractAgent> Experiment::cloneAgents(const QVector<AbstractAgent>& agents) const
{
    QVector<AbstractAgent> cloned;
    cloned.reserve(agents.size());
    foreach (AbstractAgent a, agents) {
        cloned.push_back(a.clone());
    }
    return cloned;
}
