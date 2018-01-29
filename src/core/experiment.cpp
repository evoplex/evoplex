/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>

#include "agent.h"
#include "agentsgenerator.h"
#include "experiment.h"
#include "project.h"
#include "utils.h"

namespace evoplex
{

Experiment::Experiment(MainApp* mainApp, int id, int projId, ExperimentInputs* inputs)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_projId(projId)
    , m_expStatus(INVALID)
{
    init(inputs);
}

Experiment::~Experiment()
{
    deleteTrials();
    qDeleteAll(m_fileOutputs);
    delete m_generalAttrs;
    m_generalAttrs = nullptr;
    delete m_modelAttrs;
    m_modelAttrs = nullptr;
    delete m_graphAttrs;
    m_graphAttrs = nullptr;
}

bool Experiment::init(ExperimentInputs* inputs)
{
    if (m_expStatus == RUNNING) {
        qWarning() << "[Experiment]: tried to init() a running experiment.";
        delete inputs;
        return false;
    } else if (m_expStatus != INVALID) {
        m_expStatus = INVALID;
        deleteTrials();
        qDeleteAll(m_fileOutputs);
        delete m_generalAttrs;
        delete m_modelAttrs;
        delete m_graphAttrs;
    }

    m_generalAttrs = inputs->generalAttrs;
    m_modelAttrs = inputs->modelAttrs;
    m_graphAttrs = inputs->graphAttrs;
    m_fileOutputs = inputs->fileOutputs;
    delete inputs;

    m_fileHeader = "";
    if (!m_fileOutputs.empty()) {
        for (Output* output : m_fileOutputs) {
            Q_ASSERT(output->allInputs().size() > 0);
            m_fileHeader += output->printableHeader(',') + ",";
        }
        m_fileHeader.chop(1);
        m_fileHeader += "\n";
    }

    m_numTrials = m_generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt();
    m_autoDelete = m_generalAttrs->value(GENERAL_ATTRIBUTE_AUTODELETE).toBool();
    m_stopAt = m_generalAttrs->value(GENERAL_ATTRIBUTE_STOPAT).toInt();

    m_graphPlugin = m_mainApp->graph(m_generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHID).toQString());
    m_modelPlugin = m_mainApp->model(m_generalAttrs->value(GENERAL_ATTRIBUTE_MODELID).toQString());

    m_pauseAt = m_stopAt;
    m_progress = 0;
    m_trials.reserve(m_numTrials);
    m_expStatus = READY;

    return true;
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

    m_stopAt = m_generalAttrs->value(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    m_progress = 0;

    emit (m_mainApp->expMgr()->statusChanged(this));
    emit (m_mainApp->expMgr()->restarted(this));

}

void Experiment::deleteTrials()
{
    QHash<int,QTextStream*>::iterator it;
    for (it = m_fileStreams.begin(); it != m_fileStreams.end(); ++it) {
        it.value()->flush();
        delete it.value();
    }

    for (auto& trial : m_trials) {
        delete trial.second;
    }
    m_trials.clear();

    qDeleteAll(m_clonableAgents);
    m_clonableAgents.clear();
    Agents().swap(m_clonableAgents);

    // if the experiment has finished or became invalid,
    // it's more interesing to do NOT change the status
    if (m_expStatus != FINISHED && m_expStatus != INVALID) {
        setExpStatus(READY);
        emit (m_mainApp->expMgr()->statusChanged(this));
    }

    emit (m_mainApp->expMgr()->trialsDeleted(this));
}

void Experiment::updateProgressValue()
{
    if (m_expStatus == FINISHED) {
        m_progress = 360;
    } else if (m_expStatus == INVALID) {
        m_progress = 0;
    } else if (m_expStatus == RUNNING) {
        float p = 0.f;
        for (auto& trial : m_trials) {
            p += ((float) trial.second->m_currStep / m_pauseAt);
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
        m_mainApp->expMgr()->removeFromQueue(this);
    }
}

void Experiment::playNext()
{
    if (m_expStatus != READY) {
        return;
    } else if (m_trials.empty()) {
        setPauseAt(-1);
    } else {
        int maxCurrStep = 0;
        for (auto& trial : m_trials) {
            int currStep = trial.second->m_currStep;
            if (currStep > maxCurrStep) maxCurrStep = currStep;
        }
        setPauseAt(maxCurrStep);
    }
    m_mainApp->expMgr()->play(this);
}

void Experiment::processTrial(const int& trialId)
{
    if (m_expStatus == INVALID) {
        return;
    } else if (m_trials.find(trialId) == m_trials.end()) {
        AbstractModel* trial = createTrial(trialId);
        if (!trial) {
            setExpStatus(INVALID);
            pause();
            return;
        }
        m_trials.insert({trialId, trial});
        emit (m_mainApp->expMgr()->trialCreated(this, trialId));
    }

    AbstractModel* trial = m_trials.at(trialId);
    if (trial->m_status != READY) {
        return;
    }

    trial->m_status = RUNNING;

    bool algorithmConverged = false;
    while (trial->m_currStep <= m_pauseAt && !algorithmConverged) {
        for (Output* output : m_fileOutputs)
            output->doOperation(trialId, trial);
        for (Output* output : m_extraOutputs)
            output->doOperation(trialId, trial);

        // TODO: write only after X steps
        writeStep(trialId);

        algorithmConverged = trial->algorithmStep();
        ++trial->m_currStep;
    }

    if (trial->m_currStep > m_stopAt || algorithmConverged) {
        if (!m_fileStreams.empty()) {
            for (QTextStream* stream : m_fileStreams) {
                stream->flush();
            }
        }
        trial->m_status = FINISHED;
    } else {
        trial->m_status = READY;
    }
}

AbstractModel* Experiment::createTrial(const int trialId)
{
    if (m_expStatus == INVALID) {
        return nullptr;
    } if (m_trials.size() == m_numTrials) {
        qWarning() << "[Experiment]: all the trials for this experiment have already been created."
                   << "Project:" << m_projId << "Experiment:" << m_id << "Trial:" << trialId
                   << " (" << m_trials.size() << "/" << numTrials() << ")";
        return nullptr;
    }

    m_mutex.lock(); // make it thread-safe
    Agents agents = createAgents();
    m_mutex.unlock();
    if (agents.empty()) {
        return nullptr;
    }

    const int seed = m_generalAttrs->value(GENERAL_ATTRIBUTE_SEED).toInt();
    PRG* prg = new PRG(seed + trialId);

    AbstractGraph* graphObj = m_graphPlugin->create();
    QString gType = m_generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHTYPE).toString();
    if (!graphObj || !graphObj->setup(prg, agents, m_graphAttrs, gType) || !graphObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete graphObj;
        graphObj = nullptr;
        delete prg;
        prg = nullptr;
        return nullptr;
    }
    graphObj->reset();

    AbstractModel* modelObj = m_modelPlugin->create();
    if (!modelObj || !modelObj->setup(prg, graphObj, m_modelAttrs) || !modelObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_projId << "Experiment:" << m_id;
        delete modelObj;
        return nullptr;
    }

    if (!m_fileOutputs.empty()) {
        const QString fpath = QString("%1/%2_e%3_t%4.csv")
                .arg(m_generalAttrs->value(OUTPUT_DIR).toQString())
                .arg(m_mainApp->project(m_projId)->name())
                .arg(m_id)
                .arg(trialId);

        QFile* file = new QFile(fpath);
        if (!file->open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "[Experiment] unable to create the trials."
                       << "Could not write in " << fpath;
            delete modelObj;
            return nullptr;
        }

        QTextStream* stream = new QTextStream(file);
        stream->operator <<(m_fileHeader);
        m_fileStreams.insert(trialId, stream);
    }

    modelObj->m_status = READY;
    return modelObj;
}

Agents Experiment::createAgents()
{
    if (m_expStatus == INVALID) {
        return Agents();
    } else if (!m_clonableAgents.empty()) {
        if (m_trials.size() == m_numTrials - 1) {
            Agents agents = m_clonableAgents;
            Agents().swap(m_clonableAgents);
            return agents;
        }
        return cloneAgents(m_clonableAgents);
    }

    Q_ASSERT(m_trials.empty());

    Agents agents;
    QString errMsg;
    AgentsGenerator* ag = AgentsGenerator::parse(m_modelPlugin->agentAttrSpace(),
                m_generalAttrs->value(GENERAL_ATTRIBUTE_AGENTS).toQString(), errMsg);
    if (ag) {
        agents = ag->create();
        delete ag;
    }

    if (agents.empty()) {
        errMsg = QString("[Experiment]: unable to create the trials."
                         "The set of agents could not be created (%1)."
                         "Project: %2 Experiment: %3")
                         .arg(errMsg).arg(m_projId).arg(m_id);
        qWarning() << errMsg;
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

AbstractModel* Experiment::trial(int trialId) const
{
    auto it = m_trials.find(trialId);
    if (it == m_trials.end() || !it->second)
        return nullptr;
    return it->second;
}

void Experiment::writeStep(const int trialId)
{
    // the cacheId for fileOutputs is always 0
    const int cacheId = 0;
    if (m_fileOutputs.empty() || m_fileOutputs.front()->isEmpty(cacheId, trialId)) {
        return;
    }

    QString rows;
    while (!m_fileOutputs.front()->isEmpty(cacheId, trialId)) {
        for (Output* output : m_fileOutputs) {
            Values vals = output->readFrontRow(cacheId, trialId).second;
            output->flushFrontRow(cacheId, trialId);
            for (Value val : vals) {
                rows += val.toQString() + ",";
            }
        }
        rows.chop(1);
        rows += "\n";
    }

    m_fileStreams.value(trialId)->operator <<(rows);
}

bool Experiment::removeOutput(Output* output)
{
    if (m_expStatus != Experiment::READY) {
        qWarning() << "[Experiment] : tried to remove an 'Output' from a running experiment. You should pause it first.";
        return false;
    }

    std::vector<Output*>::iterator it;
    it = std::find(m_extraOutputs.begin(), m_extraOutputs.end(), output);
    if (it == m_extraOutputs.end()) {
        qWarning() << "[Experiment] : tried to remove a non-existent 'Output'.";
        return false;
    }

    if (!(*it)->isEmpty()) {
        qWarning() << "[Experiment] : tried to remove an 'Output' that seems to be used somewhere. It should be cleaned first.";
        return false;
    }

    m_extraOutputs.erase(it);
    delete *it;
    *it = nullptr;

    return true;
}

Output* Experiment::searchOutput(const Output* find)
{
    for (Output* output : m_fileOutputs) {
        if (output->operator ==(find)) {
            return output;
        }
    }
    for (Output* output : m_extraOutputs) {
        if (output->operator ==(find)) {
            return output;
        }
    }
    return nullptr;
}

Experiment::ExperimentInputs* Experiment::readInputs(const MainApp* mainApp,
        const QStringList& header, const QStringList& values, QString& errorMsg)
{
    if (header.isEmpty() || values.isEmpty() || header.size() != values.size()) {
        errorMsg = "The 'header' and 'values' cannot be empty and must have the same number of elements.";
        return nullptr;
    }

    // find the model and graph for this experiment
    const int headerGraphId = header.indexOf(GENERAL_ATTRIBUTE_GRAPHID);
    const int headerModelId = header.indexOf(GENERAL_ATTRIBUTE_MODELID);
    if (headerGraphId < 0 && headerModelId < 0) {
        errorMsg = "The experiment should have both graphId and modelId.";
        return nullptr;
    }

    // check if the model and graph are available
    const GraphPlugin* gPlugin = mainApp->graph(values.at(headerGraphId));
    const ModelPlugin* mPlugin = mainApp->model(values.at(headerModelId));
    if (!gPlugin || !mPlugin) {
        errorMsg = QString("The graphId (%1) or modelId (%2) are not available."
                           " Make sure to load them before trying to add this experiment.")
                           .arg(values.at(headerGraphId)).arg(values.at(headerModelId));
        return nullptr;
    }

    // make sure that the chosen graphId is allowed in this model
    if (!mPlugin->supportedGraphs().contains(gPlugin->id())) {
        QString supportedGraphs = mPlugin->supportedGraphs().toList().join(", ");
        errorMsg = QString("The graphId (%1) cannot be used in this model (%2). The allowed ones are: %3")
                           .arg(gPlugin->id()).arg(mPlugin->id()).arg(supportedGraphs);
        return nullptr;
    }

    // we assume that all graph/model attributes start with 'uid_'
    const QString& graphId_ = gPlugin->id() + "_";
    const QString& modelId_ = mPlugin->id() + "_";

    // get the value of each attribute and make sure they are valid
    QStringList failedAttributes;
    Attributes* generalAttrs = new Attributes(mainApp->generalAttrSpace().size());
    Attributes* modelAttrs = new Attributes(mPlugin->pluginAttrSpace().size());
    Attributes* graphAttrs = new Attributes(gPlugin->pluginAttrSpace().size());
    for (int i = 0; i < values.size(); ++i) {
        const QString& vStr = values.at(i);
        QString attrName = header.at(i);

        AttributesSpace::const_iterator gps = mainApp->generalAttrSpace().find(attrName);
        if (gps != mainApp->generalAttrSpace().end()) {
            Value value = gps.value()->validate(vStr);
            if (value.isValid()) {
                generalAttrs->replace(gps.value()->id(), attrName, value);
            } else {
                failedAttributes.append(attrName);
            }
        } else {
            ValueSpace* valSpace = nullptr;
            Attributes* attributes = nullptr;
            if (attrName.startsWith(modelId_)) {
                attrName = attrName.remove(modelId_);
                valSpace = mPlugin->pluginAttrSpace().value(attrName);
                attributes = modelAttrs;
            } else if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                valSpace = gPlugin->pluginAttrSpace().value(attrName);
                attributes = graphAttrs;
            } else {
                qFatal("[Experiment::readInputs()]: attribute name should start with the plugin id!");
            }

            if (attributes && valSpace) {
                Value value = valSpace->validate(vStr);
                if (value.isValid()) {
                    attributes->replace(valSpace->id(), attrName, value);
                } else {
                    failedAttributes.append(attrName);
                }
            }
        }
    }

    QString outHeader = generalAttrs->value(OUTPUT_HEADER).toQString();
    std::vector<Output*> outputs;
    if (!outHeader.isEmpty()) {
        const int numTrials = generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt();
        Q_ASSERT(numTrials > 0);
        std::vector<int> trialIds;
        for (int i = 0; i < numTrials; ++i) {
            trialIds.emplace_back(i);
        }

        outputs = Output::parseHeader(outHeader.split(";"), trialIds, mPlugin, errorMsg);
        if (outputs.empty()) {
            failedAttributes.append(OUTPUT_HEADER);
        }

        QFileInfo outDir(generalAttrs->value(OUTPUT_DIR).toQString());
        if (!outDir.isDir() || !outDir.isWritable()) {
            errorMsg += "The output directory must be valid and writable!\n";
            failedAttributes.append(OUTPUT_DIR);
        }
    }

    if (!failedAttributes.isEmpty()) {
        errorMsg += QString("The following attributes are missing/invalid: %1").arg(failedAttributes.join(","));
        delete generalAttrs;
        delete graphAttrs;
        delete modelAttrs;
        qDeleteAll(outputs);
        return nullptr;
    }

    // make sure all attributes exist
    Q_ASSERT(generalAttrs->indexOf("") == -1);
    Q_ASSERT(modelAttrs->indexOf("") == -1);
    Q_ASSERT(graphAttrs->indexOf("") == -1);

    // that's great! everything seems to be valid
    ExperimentInputs* inputs = new ExperimentInputs;
    inputs->generalAttrs = generalAttrs;
    inputs->modelAttrs = modelAttrs;
    inputs->graphAttrs = graphAttrs;
    inputs->fileOutputs = outputs;

    return inputs;
}

} // evoplex
