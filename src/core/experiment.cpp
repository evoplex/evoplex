/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QThread>

#include "experiment.h"
#include "agent.h"
#include "agentsgenerator.h"
#include "project.h"

namespace evoplex
{

Experiment::Experiment(MainApp* mainApp, ExperimentInputs* inputs, ProjectSP project)
    : m_mainApp(mainApp)
    , m_id(inputs->generalAttrs->value(GENERAL_ATTRIBUTE_EXPID).toInt())
    , m_project(project)
    , m_inputs(nullptr)
    , m_expStatus(INVALID)
{
    QString error;
    init(inputs, error);
    Q_ASSERT(m_project);
}

Experiment::~Experiment()
{
    Q_ASSERT(m_expStatus != RUNNING && m_expStatus != QUEUED);
    deleteTrials();
    m_outputs.clear();
    delete m_inputs;
    m_project.clear();
}

bool Experiment::init(ExperimentInputs* inputs, QString& error)
{
    if (m_expStatus == RUNNING || m_expStatus == QUEUED) {
        error = "Tried to initialize a running experiment.\n"
                "Please, pause it and try again.";
        qWarning() << "[Experiment]:" << error;
        return false;
    }

    m_outputs.clear();
    delete m_inputs;
    m_inputs = inputs;

    m_filePathPrefix.clear();
    m_fileHeader.clear();
    if (!m_inputs->fileCaches.empty()) {
        m_filePathPrefix = QString("%1/%2_e%3_t")
                .arg(m_inputs->generalAttrs->value(OUTPUT_DIR).toQString())
                .arg(m_project->name())
                .arg(m_id);

        for (Cache* cache : m_inputs->fileCaches) {
            Q_ASSERT(cache->inputs().size() > 0);
            m_fileHeader += cache->printableHeader(',', false) + ",";
            m_outputs.insert(cache->output());
        }
        m_fileHeader.chop(1);
        m_fileHeader += "\n";
    }

    m_numTrials = m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt();
    m_autoDeleteTrials = m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_AUTODELETE).toBool();

    m_graphPlugin = m_mainApp->graph(m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHID).toQString());
    m_modelPlugin = m_mainApp->model(m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_MODELID).toQString());

    reset();

    return true;
}

void Experiment::reset()
{
    if (m_expStatus == RUNNING || m_expStatus == QUEUED) {
        qWarning() << "[Experiment]: tried to reset a running experiment. You should pause it first.";
        return;
    }

    deleteTrials();

    QMutexLocker locker(&m_mutex);

    for (OutputSP o : m_outputs) {
        o->flushAll();
    }

    m_trials.reserve(m_numTrials);
    m_delay = m_mainApp->defaultStepDelay();
    m_stopAt = m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    m_progress = 0;
    m_expStatus = READY;

    emit (m_mainApp->expMgr()->statusChanged(this));
    emit (restarted());
}

void Experiment::deleteTrials()
{
    QMutexLocker locker(&m_mutex);

    for (auto& trial : m_trials) {
        delete trial.second;
    }
    m_trials.clear();

    Utils::deleteAndShrink(m_clonableAgents);
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
        setPauseAt(-1); // just create and set the trials
    } else {
        int maxCurrStep = 0;
        for (const auto& trial : m_trials) {
            int currStep = trial.second->m_currStep;
            if (currStep > maxCurrStep) maxCurrStep = currStep;
        }
        setPauseAt(maxCurrStep + 1);
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
            pause();
            return;
        }
        m_trials.insert({trialId, trial});
        emit (trialCreated(trialId));
    }

    AbstractModel* trial = m_trials.at(trialId);
    if (trial->m_status != READY) {
        return;
    }

    trial->m_status = RUNNING;

    QElapsedTimer t;
    t.start();

    bool algorithmConverged = false;
    while (trial->m_currStep < m_pauseAt && !algorithmConverged) {
        algorithmConverged = trial->algorithmStep();
        ++trial->m_currStep;

        for (OutputSP output : m_outputs)
            output->doOperation(trialId, trial);

        if (m_inputs->fileCaches.size()
                && trial->m_currStep % m_mainApp->stepsToFlush() == 0
                && !writeCachedSteps(trialId)) {
            trial->m_status = INVALID;
            setExpStatus(INVALID);
            pause();
            return;
        }

        if (m_delay > 0)
            QThread::msleep(m_delay);
    }

    qDebug() << QString("%1 (E%2:T%3) - %4s")
                .arg(m_project->name()).arg(m_id).arg(trialId)
                .arg(t.elapsed() / 1000);

    if (trial->m_currStep >= m_stopAt || algorithmConverged) {
        if (writeCachedSteps(trialId)) {
            trial->m_status = FINISHED;
        } else {
            trial->m_status = INVALID;
            setExpStatus(INVALID);
            pause();
        }
    } else {
        trial->m_status = READY;
    }
}

AbstractModel* Experiment::createTrial(const int trialId)
{
    // Make it thread-safe to make sure that we create one trial at a time.
    // Thus, if one trial fail, then the other will be aborted earlier.
    QMutexLocker locker(&m_mutex);

    if (m_expStatus == INVALID || m_pauseAt == 0) {
        return nullptr;
    } if (m_trials.size() == m_numTrials) {
        m_expStatus = INVALID;
        QString e = QString("[Experiment]: FATAL! all the trials for this experiment have already been created."
                            "It should never happen!\n Project: %1; Exp: %2; Trial: %3 (max=%4)\n")
                            .arg(m_project->name()).arg(m_id).arg(trialId).arg(m_numTrials);
        qFatal(qUtf8Printable(e));
        return nullptr;
    }

    Agents agents = createAgents();
    if (agents.empty()) {
        m_expStatus = INVALID;
        return nullptr;
    }

    const int seed = m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_SEED).toInt();
    PRG* prg = new PRG(seed + trialId);

    AbstractGraph* graphObj = m_graphPlugin->create();
    QString gType = m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_GRAPHTYPE).toString();
    if (!graphObj || !graphObj->setup(prg, agents, m_inputs->graphAttrs, gType) || !graphObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_project->name() << "Experiment:" << m_id;
        m_expStatus = INVALID;
        delete graphObj;
        delete prg;
        return nullptr;
    }
    graphObj->reset();

    AbstractModel* modelObj = m_modelPlugin->create();
    if (!modelObj || !modelObj->setup(prg, graphObj, m_inputs->modelAttrs) || !modelObj->init()) {
        qWarning() << "[Experiment]: unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_project->name() << "Experiment:" << m_id;
        m_expStatus = INVALID;
        delete modelObj;
        return nullptr;
    }

    if (!m_inputs->fileCaches.empty()) {
        const QString fpath = m_filePathPrefix + QString("%4.csv").arg(trialId);
        QFile file(fpath);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream stream(&file);
            stream << m_fileHeader;
            file.close();
        } else {
            qWarning() << "[Experiment] unable to create the trials."
                       << "Could not write in " << fpath;
            m_expStatus = INVALID;
            delete modelObj;
            return nullptr;
        }

        // write this initial step to file
        for (OutputSP output : m_outputs) {
            output->doOperation(trialId, modelObj);
        }
        writeCachedSteps(trialId);
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
                m_inputs->generalAttrs->value(GENERAL_ATTRIBUTE_AGENTS).toQString(), errMsg);
    if (ag) {
        agents = ag->create();
        delete ag;
    }

    if (agents.empty()) {
        errMsg = QString("[Experiment]: unable to create the trials."
                         "The set of agents could not be created (%1)."
                         "Project: %2 Experiment: %3")
                         .arg(errMsg).arg(m_project->name()).arg(m_id);
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

bool Experiment::writeCachedSteps(const int trialId)
{
    if (m_inputs->fileCaches.empty() || m_inputs->fileCaches.front()->isEmpty(trialId)) {
        return true;
    }

    const QString fpath = m_filePathPrefix + QString("%1.csv").arg(trialId);
    QFile file(fpath);
    if (!file.open(QFile::WriteOnly | QFile::Append)) {
        qWarning() << "[Experiment] unable to create the trials."
                   << "Could not write in " << fpath;
        return false;
    }

    QTextStream stream(&file);
    do {
        QString row;
        for (Cache* cache : m_inputs->fileCaches) {
            Values vals = cache->readFrontRow(trialId).second;
            cache->flushFrontRow(trialId);
            for (Value val : vals) {
                row += val.toQString() + ",";
            }
        }
        row.chop(1);
        stream << row << "\n";

    // we synchronously flush all the io stuff. So, it's safe to say
    // that if the front Output is empty, then all others are also empty.
    } while (!m_inputs->fileCaches.front()->isEmpty(trialId));

    file.close();
    return true;
}

bool Experiment::removeOutput(OutputSP output)
{
    if (m_expStatus != Experiment::READY) {
        qWarning() << "[Experiment] : tried to remove an 'Output' from a running experiment. You should pause it first.";
        return false;
    }

    if (!output->isEmpty()) {
        qWarning() << "[Experiment] : tried to remove an 'Output' that seems to be used somewhere. It should be cleaned first.";
        return false;
    }

    std::unordered_set<OutputSP>::iterator it = m_outputs.find(output);
    if (it == m_outputs.end()) {
        qWarning() << "[Experiment] : tried to remove a non-existent 'Output'.";
        return false;
    }

    m_outputs.erase(it);
    return true;
}

OutputSP Experiment::searchOutput(const OutputSP find)
{
    for (OutputSP output : m_outputs) {
        if (output->operator==(find)) {
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
    if (!mPlugin->graphIsSupported(gPlugin->id())) {
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
            Attributes* pluginAttrs = nullptr;
            if (attrName.startsWith(modelId_)) {
                attrName = attrName.remove(modelId_);
                valSpace = mPlugin->pluginAttrSpace().value(attrName);
                pluginAttrs = modelAttrs;
            } else if (attrName.startsWith(graphId_)) {
                attrName = attrName.remove(graphId_);
                valSpace = gPlugin->pluginAttrSpace().value(attrName);
                pluginAttrs = graphAttrs;
            }

            if (pluginAttrs) {
                Value value;
                if (valSpace) {
                    value = valSpace->validate(vStr);
                }

                if (value.isValid()) {
                    pluginAttrs->replace(valSpace->id(), attrName, value);
                } else {
                    failedAttributes.append(attrName);
                }
            }
        }
    }

    std::vector<Cache*> fileCaches;
    QString outHeader = generalAttrs->value(OUTPUT_HEADER, Value("")).toQString();
    if (failedAttributes.isEmpty() && !outHeader.isEmpty()) {
        const int numTrials = generalAttrs->value(GENERAL_ATTRIBUTE_TRIALS).toInt();
        Q_ASSERT(numTrials > 0);
        std::vector<int> trialIds;
        for (int i = 0; i < numTrials; ++i) {
            trialIds.emplace_back(i);
        }

        fileCaches = Output::parseHeader(outHeader.split(";", QString::SkipEmptyParts), trialIds, mPlugin, errorMsg);
        if (fileCaches.empty()) {
            failedAttributes.append(OUTPUT_HEADER);
        }

        QFileInfo outDir(generalAttrs->value(OUTPUT_DIR, Value("")).toQString());
        if (!outDir.isDir() || !outDir.isWritable()) {
            errorMsg += "The output directory must be valid and writable!\n";
            failedAttributes.append(OUTPUT_DIR);
        }
    }

    // make sure all attributes exist
    auto checkAll = [&failedAttributes](const Attributes* attrs, const AttributesSpace& attrSpace) {
        for (const ValueSpace* valSpace : attrSpace) {
            if (!attrs->contains(valSpace->attrName())) {
                failedAttributes.append(valSpace->attrName());
            }
        }
    };
    checkAll(generalAttrs, mainApp->generalAttrSpace());
    checkAll(modelAttrs, mPlugin->pluginAttrSpace());
    checkAll(graphAttrs, gPlugin->pluginAttrSpace());

    ExperimentInputs* ei = new ExperimentInputs(generalAttrs, modelAttrs, graphAttrs, fileCaches);

    if (!failedAttributes.isEmpty()) {
        errorMsg += QString("The following attributes are missing/invalid: %1").arg(failedAttributes.join(","));
        delete ei;
        return nullptr;
    }

    // that's great! everything seems to be valid
    return ei;
}

} // evoplex
