/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>

#include "agent.h"
#include "experiment.h"
#include "project.h"
#include "filemgr.h"
#include "utils.h"

namespace evoplex {

Experiment::Experiment(MainApp* mainApp, int id, int projId, Attributes* generalAttrs,
        Attributes* modelAttrs, Attributes* graphAttrs, std::vector<Output*> fileOutputs)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_projId(projId)
    , m_expStatus(INVALID)
{
    init(generalAttrs, modelAttrs, graphAttrs, fileOutputs);
}

Experiment::~Experiment()
{
    deleteTrials();
    delete m_generalAttrs;
    m_generalAttrs = nullptr;
    delete m_modelAttrs;
    m_modelAttrs = nullptr;
    delete m_graphAttrs;
    m_graphAttrs = nullptr;

    for (QTextStream* stream : m_fileStreams) {
        stream->flush();
        delete stream;
        stream = nullptr;
    }
    qDeleteAll(m_fileOutputs);
}

void Experiment::init(Attributes* generalAttrs, Attributes* modelAttrs,
                      Attributes* graphAttrs, std::vector<Output*> fileOutputs)
{
    m_generalAttrs = generalAttrs;
    m_modelAttrs = modelAttrs;
    m_graphAttrs = graphAttrs;
    m_fileOutputs = fileOutputs;
    m_fileHeader = "";

    if (!m_fileOutputs.empty()) {
        for (Output* output : m_fileOutputs) {
            m_fileHeader += output->printableHeader() + ",";
        }
        m_fileHeader.chop(1);
        m_fileHeader += "\n";
    }

    m_numTrials = generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt;
    m_seed = generalAttrs->value(GENERAL_ATTRIBUTE_SEED).toInt;
    m_autoDelete = generalAttrs->value(GENERAL_ATTRIBUTE_AUTODELETE).toBool;
    m_stopAt = generalAttrs->value(GENERAL_ATTRIBUTE_STOPAT).toInt;

    m_graphPlugin = m_mainApp->getGraph(generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHID).toQString());
    m_modelPlugin = m_mainApp->getModel(generalAttrs->value(GENERAL_ATTRIBUTE_MODELID).toQString());

    m_pauseAt = m_stopAt;
    m_progress = 0;
    m_trials.reserve(m_numTrials);
    m_expStatus = READY;
}

void Experiment::reset()
{
    if (m_expStatus == RUNNING) {
        qWarning() << "[Experiment]: tried to reset a running experiment. You should pause it first.";
        return;
    }
    deleteTrials();
    m_trials.reserve(m_numTrials);
    m_expStatus = READY;
    emit (m_mainApp->getExperimentsMgr()->statusChanged(this));
}

void Experiment::deleteTrials()
{
    QHash<int, Trial>::iterator it;
    for (it = m_trials.begin(); it != m_trials.end(); ++it) {
        delete it.value().modelObj;
    }
    m_trials.clear();
    m_trials.squeeze();

    // if the experiment has finished or became invalid,
    // it's more interesing to do NOT change the status
    if (m_expStatus != FINISHED && m_expStatus != INVALID) {
        setExpStatus(READY);
    }
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
    if (m_expStatus == RUNNING) {
        pause();
    } else if (m_expStatus == READY) {
        play();
    } else if (m_expStatus == QUEUED) {
        m_mainApp->getExperimentsMgr()->removeFromQueue(this);
    }
}

void Experiment::playNext()
{
    if (m_expStatus != READY) {
        return;
    } else if (m_trials.isEmpty()) {
        setPauseAt(1);
    } else {
        setPauseAt(m_trials.value(0).currentStep + 1);
    }
    m_mainApp->getExperimentsMgr()->play(this);
}

void Experiment::processTrial(const int& trialId)
{
    if (m_expStatus == INVALID) {
        return;
    } else if (!m_trials.contains(trialId)) {
        Trial trial = createTrial(trialId);
        if (trial.status == INVALID) {
            setExpStatus(INVALID);
            pause();
            return;
        }
        m_trials.insert(trialId, trial);
        writeStep(trialId, trial.modelObj);
    }

    Trial& trial = m_trials[trialId];
    if (trial.status != READY) {
        return;
    }

    trial.status = RUNNING;

    bool algorithmConverged = false;
    while (trial.currentStep < m_pauseAt && !algorithmConverged) {
        algorithmConverged = trial.modelObj->algorithmStep();
        writeStep(trialId, trial.modelObj);
        ++trial.currentStep;
    }

    if (trial.currentStep >= m_stopAt || algorithmConverged) {
        for (QTextStream* stream : m_fileStreams) {
            stream->flush();
        }
        trial.status = FINISHED;
    } else {
        trial.status = READY;
    }
}

Experiment::Trial Experiment::createTrial(const int trialId)
{
    if (m_expStatus == INVALID) {
        return Trial();
    } if (m_trials.size() == m_numTrials) {
        qWarning() << "[Experiment]: all the trials for this experiment have already been created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        return Trial();
    }

    m_mutex.lock(); // make it thread-safe
    Agents agents = createAgents();
    m_mutex.unlock();
    if (agents.empty()) {
        return Trial();
    }

    PRG* prg = new PRG(m_seed + trialId);
    AbstractGraph* graphObj = m_graphPlugin->factory->create();
    graphObj->setup(prg, agents, m_graphAttrs);
    if (!graphObj || !graphObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        graphObj = nullptr;
        delete prg;
        prg = nullptr;
        return Trial();
    }
    graphObj->reset();

    AbstractModel* modelObj = m_modelPlugin->factory->create();
    modelObj->setup(prg, graphObj, m_modelAttrs);
    if (!modelObj || !modelObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete modelObj;
        modelObj = nullptr;
        return Trial();
    }

    if (!m_fileOutputs.empty()) {
        const QString fpath = QString("%1/%2_e%3_t%4.csv")
                .arg(m_generalAttrs->value(OUTPUT_DIR).toQString())
                .arg(m_mainApp->getProject(m_projId)->getName())
                .arg(m_id)
                .arg(trialId);

        QFile* file = new QFile(fpath);
        if (!file->open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "[Experiment] unable to create the trials."
                       << "Could not write in " << fpath;
            delete modelObj;
            modelObj = nullptr;
            return Trial();
        }

        QTextStream* stream = new QTextStream(file);
        stream->operator <<(m_fileHeader);
        m_fileStreams.insert(trialId, stream);
    }

    Trial trial;
    trial.modelObj = modelObj;
    trial.status = READY;
    return trial;
}

Agents Experiment::createAgents()
{
    if (m_expStatus == INVALID) {
        return Agents();
    } else if (!m_clonableAgents.empty()) {
        if (m_trials.size() == m_numTrials - 1) {
            Agents agents = m_clonableAgents;
            m_clonableAgents = Agents();
            return agents;
        }
        return cloneAgents(m_clonableAgents);
    }

    Q_ASSERT(m_trials.size() == 0);

    Agents agents;
    bool isInt;
    int numAgents = m_generalAttrs->value(GENERAL_ATTRIBUTE_AGENTS).toQString().toInt(&isInt);
    if (isInt) { // create a population of agents with random properties?
        if (numAgents > 0) {
            PRG* prg = new PRG(m_seed);
            QVector<Attributes> attrs = Utils::randomAttrs(m_modelPlugin->agentAttrSpace, prg, numAgents);
            delete prg;
            Q_ASSERT(attrs.size() == numAgents);
            agents.reserve(numAgents);
            for (int id = 0; id < numAgents; ++id) {
                agents.emplace_back(new Agent(id, attrs.at(id)));
            }
        }
    } else { // read population from a text file
        agents = m_mainApp->getFileMgr()->importAgents(
                    m_generalAttrs->value(GENERAL_ATTRIBUTE_AGENTS).toQString(),
                    m_modelPlugin->uid);
    }

    if (agents.empty()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The set of agents could not be created."
                   << "Project:" << m_projId << "Experiment:" << m_id;
    } else if (m_numTrials > 1) {
        m_clonableAgents = cloneAgents(agents);
    }

    return agents;
}

Agents Experiment::cloneAgents(const Agents& agents) const
{
    Agents cloned;
    cloned.reserve(agents.size());
    for (Agents::const_iterator it = agents.begin(); it != agents.end(); ++it) {
        cloned.emplace_back((*it)->clone());
    }
    return cloned;
}

AbstractGraph* Experiment::graph(int trialId) const
{
    QHash<int, Trial>::const_iterator it = m_trials.find(trialId);
    if (it == m_trials.end() || !it.value().modelObj)
        return nullptr;
    return it.value().modelObj->graph();
}

void Experiment::writeStep(const int trialId, AbstractModel* modelObj)
{
    if (m_fileOutputs.empty()) {
        return;
    }

    QString values;
    for (Output* output : m_fileOutputs) {
        std::vector<Value> vals = output->doOperation(modelObj);
        for (Value val : vals) {
            values += val.toQString() + ",";
        }
    }
    values.chop(1);
    values += "\n";
    m_fileStreams.value(trialId)->operator <<(values);
}
}
