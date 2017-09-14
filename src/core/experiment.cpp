/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/abstractagent.h"
#include "core/experiment.h"
#include "core/filemgr.h"
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
    , m_seed(generalParams.value(GENERAL_ATTRIBUTE_SEED).toInt())
    , m_progress(0)
    , m_autoDelete(true)
{
    m_trials.reserve(m_numTrials);
    m_stopAt = m_generalParams.value(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    m_expStatus = READY;
}

void Experiment::updateProgressValue()
{
    if (m_expStatus == FINISHED) {
        m_progress = 360;
    } else if (m_expStatus == INVALID) {
        m_progress = 0;
    } else if (m_expStatus == RUNNING) {
        float p = 0.f;
        QHash<int, Trial>::iterator it = m_trials.begin();
        while (it != m_trials.end()) {
            p += ((float) it.value().currentStep / m_pauseAt);
            ++it;
        }
        m_progress = ceil(p * 360.f / m_numTrials);
    }
}

void Experiment::toggle()
{
    if (m_expStatus == RUNNING)
        pause();
    else if (m_expStatus == READY)
        run();
}

void Experiment::processTrial(const int& trialId)
{
    if (m_expStatus == INVALID) {
        return;
    } else if (!m_trials.contains(trialId)) {
        Trial trial = createTrial(m_seed + trialId);
        if (trial.status == INVALID) {
            setExpStatus(INVALID);
            pause();
            return;
        }
        m_trials.insert(trialId, trial);
    }

    Trial& trial = m_trials[trialId];
    if (trial.status != READY) {
        return;
    }

    trial.status = RUNNING;

    bool algorithmConverged = false;
    while (trial.currentStep < m_pauseAt && !algorithmConverged) {
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

Experiment::Trial Experiment::createTrial(const int& trialSeed)
{
    if (m_expStatus == INVALID) {
        return Trial();
    } if (m_trials.size() == m_numTrials) {
        qWarning() << "[Experiment]: all the trials for this experiment have already been created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        return Trial();
    }

    m_mutex.lock();
    QVector<AbstractAgent> agents = createAgents();
    m_mutex.unlock();
    if (agents.isEmpty()) {
        agents.squeeze();
        return Trial();
    }

    AbstractGraph* graphObj = m_graphPlugin->factory->create();
    if (!graphObj || !graphObj->init(agents, m_graphParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        agents.clear();
        agents.squeeze();
        return Trial();
    }

    AbstractModel* modelObj = m_modelPlugin->factory->create();
    modelObj->setup(trialSeed, graphObj); // make the PRG and the graph available in the model
    if (!modelObj || !modelObj->init(m_modelParams)) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        delete modelObj;
        agents.clear();
        agents.squeeze();
        return Trial();
    }

    Trial trial;
    trial.modelObj = modelObj;
    trial.status = READY;
    return trial;
}

QVector<AbstractAgent> Experiment::createAgents()
{
    if (m_expStatus == INVALID) {
        return QVector<AbstractAgent>();
    } else if (!m_clonableAgents.isEmpty()) {
        if (m_trials.size() == m_numTrials - 1) {
            return m_clonableAgents;
        }
        return cloneAgents(m_clonableAgents);
    }

    Q_ASSERT(m_trials.size() == 0);

    QVector<AbstractAgent> agents;
    bool isInt;
    int numAgents = m_generalParams.value(GENERAL_ATTRIBUTE_AGENTS).toInt(&isInt);
    if (isInt) { // create a population of agents with random properties?
        PRG* prg = new PRG(m_seed);
        QVector<QVariantHash> atbs = Utils::randomParams(m_modelPlugin->agentAttrSpace, prg, numAgents);
        delete prg;
        Q_ASSERT(atbs.size() == numAgents);
        agents.reserve(numAgents);
        for (int i = 0; i < numAgents; ++i) {
            agents.push_back(AbstractAgent(atbs.at(i)));
        }
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

AbstractGraph* Experiment::getGraph(int trialId) const
{
    if (!m_trials.contains(trialId))
        return nullptr;
    return m_trials.value(trialId).modelObj->graph();
}
