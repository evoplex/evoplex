/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>

#include "experiment.h"
#include "nodes.h"
#include "nodes_p.h"
#include "project.h"
#include "trial.h"
#include "utils.h"

namespace evoplex {

Experiment::Experiment(MainApp* mainApp, const int id, ProjectWPtr project)
    : m_mainApp(mainApp),
      m_id(id),
      m_project(project),
      m_inputs(nullptr),
      m_graphType(GraphType::Invalid),
      m_numTrials(0),
      m_autoDeleteTrials(true),
      m_stopAt(-1),
      m_pauseAt(-1),
      m_progress(0),
      m_delay(mainApp->defaultStepDelay()),
      m_expStatus(Status::Invalid)
{
    Q_ASSERT_X(project.lock(), "Experiment", "an experiment must belong to a valid project");
}

Experiment::~Experiment()
{
    Q_ASSERT_X(m_expStatus != Status::Running && m_expStatus != Status::Queued,
               "Experiment", "tried to delete a running experiment");
    m_outputs.clear();
    delete m_inputs;
}

void Experiment::invalidate()
{
    if (!disable()) {
        m_mainApp->expMgr()->remove(shared_from_this());
        setAutoDeleteTrials(true);
        pause();
    }
    setExpStatus(Status::Invalid);
}

void Experiment::deleteTrials()
{
    for (auto& trial : m_trials) {
        delete trial.second;
    }
    m_trials.clear();
    m_clonableNodes.clear();
}

bool Experiment::setInputs(ExpInputsPtr inputs, QString& error)
{
    if (!disable(&error)) {
        return false;
    }

    QMutexLocker locker(&m_mutex);

    Q_ASSERT(inputs.get() != m_inputs);
    delete m_inputs;
    m_inputs = inputs.release();

    // a few asserts for critical things that should never happen!
    Q_ASSERT_X(this == m_project.lock()->experiment(m_id).get(),
        "Experiment", "an experiment must be aware of its parent project!");
    Q_ASSERT_X(m_id == m_inputs->general(GENERAL_ATTR_EXPID).toInt(),
        "Experiment", "mismatched experiment id!");
    Q_ASSERT_X(m_inputs->graphPlugin() && m_inputs->modelPlugin(),
        "Experiment", "tried to setup an experiment with invalid plugins!");

    m_graphType = _enumFromString<GraphType>(m_inputs->general(GENERAL_ATTR_GRAPHTYPE).toQString());
    if (m_graphType == GraphType::Invalid) {
        error += "the graph type is invalid!\n";
        m_expStatus = Status::Invalid;
    }

    m_numTrials = m_inputs->general(GENERAL_ATTR_TRIALS).toInt();
    if (m_numTrials < 1 || m_numTrials > EVOPLEX_MAX_TRIALS) {
        error += QString("number of trials should be >0 and <%1!\n").arg(EVOPLEX_MAX_TRIALS);
        m_expStatus = Status::Invalid;
    }

    m_autoDeleteTrials = m_inputs->general(GENERAL_ATTR_AUTODELETE).toBool();
    setStopAt(m_inputs->general(GENERAL_ATTR_STOPAT).toInt());
    setPauseAt(m_stopAt);

    if (!error.isEmpty()) {
        qWarning() << error;
    }

    emit (statusChanged(m_expStatus));
    emit (restarted());
    return m_expStatus != Status::Invalid;
}

bool Experiment::disable(QString* error)
{
    QMutexLocker locker(&m_mutex);

    if (m_expStatus == Status::Running || m_expStatus == Status::Queued) {
        QString e("Tried to disable a running experiment.\n"
                  "Please, pause it and try again.");
        qWarning() << e;
        if (error) *error = e;
        return false;
    }

    m_mainApp->expMgr()->remove(shared_from_this());

    deleteTrials();
    m_outputs.clear();
    m_filePathPrefix.clear();
    m_fileHeader.clear();
    m_expStatus = Status::Disabled;
    setProgress(0);
    return true;
}

bool Experiment::reset(QString* error)
{
    QMutexLocker locker(&m_mutex);

    QString erroMsg;
    if (!m_inputs || m_expStatus == Status::Invalid) {
        erroMsg = "Tried to reset an invalid experiment.\n"
                  "Please, check its inputs, click on Edit and try again.";
    } else if (m_expStatus == Status::Running || m_expStatus == Status::Queued) {
        erroMsg = "Tried to reset a running experiment.\n"
                  "Please, pause it and try again.";
    } else if (m_expStatus == Status::Disabled) {
        enable(erroMsg);
    }

    if (!m_inputs || !erroMsg.isEmpty()) {
        qWarning() << erroMsg;
        if (error) *error = erroMsg;
        return false;
    }

    m_stopAt = m_inputs->general(GENERAL_ATTR_STOPAT).toInt();
    setProgress(0);

    for (auto const& o : m_outputs) {
        o->flushAll();
    }

    deleteTrials();
    m_trials.reserve(static_cast<size_t>(m_numTrials));
    for (quint16 trialId = 0; trialId < m_numTrials; ++trialId) {
        m_trials.insert({trialId, new Trial(trialId, shared_from_this())});
    }

    m_expStatus = Status::Paused;
    emit (statusChanged(m_expStatus));

    emit (restarted());
    return true;
}

void Experiment::enable(QString& error)
{
    Q_ASSERT(m_inputs && m_expStatus == Status::Disabled);

    ProjectPtr project = m_project.lock();
    if (!project) {
        error += "experiment does not belong to a valid project";
        m_expStatus = Status::Invalid;
        emit (statusChanged(m_expStatus));
        return;
    }

    if (m_inputs->fileCaches().empty()) {
        return; // nothing to do
    }

    m_filePathPrefix = QString("%1/%2_e%3_t")
            .arg(m_inputs->general(OUTPUT_DIR).toQString(), project->name())
            .arg(m_id);

    m_outputs.clear();
    m_fileHeader.clear();
    for (const Cache* cache : m_inputs->fileCaches()) {
        m_fileHeader += cache->printableHeader(',', false) + ",";
        m_outputs.insert(cache->output());
    }
    m_fileHeader.chop(1);
    m_fileHeader += "\n";
}

const Trial* Experiment::trial(quint16 trialId) const
{
    auto it = m_trials.find(trialId);
    if (it == m_trials.end())
        return nullptr;
    return it->second;
}

void Experiment::updateProgressValue()
{
    if (m_expStatus != Status::Running) {
        return;
    }

    float p = 0.f;
    for (auto const& t : m_trials) {
        p += (static_cast<float>(t.second->step()) / m_stopAt);
    }
    auto newProg = static_cast<quint16>(std::ceil(p * 360.f / m_numTrials));
    if (newProg != m_progress) {
        setProgress(newProg);
    }
}

void Experiment::trialFinished(Trial* trial)
{
    QMutexLocker locker(&m_mutex);
    if (m_expStatus != Status::Invalid && trial->status() == Status::Invalid) {
        m_expStatus = Status::Invalid;
        pause(); // pause all other trials asap
        emit (statusChanged(m_expStatus));
    }
    locker.unlock();
    m_mainApp->expMgr()->trialFinished(trial);
}

void Experiment::expFinished()
{
    QMutexLocker locker(&m_mutex);

    if (m_expStatus == Status::Invalid) {
        locker.unlock();
        disable(); // delete everything to save memory
        locker.relock();
        m_expStatus = Status::Invalid; // keep it invalid (don't need to re-emit)
        return;
    }

    bool allTrialsFinished = true;
    for (auto const& t : m_trials) {
        if (t.second->status() != Status::Finished) {
            allTrialsFinished = false;
            break;
        }
    }

    if (allTrialsFinished) {
        m_expStatus = Status::Finished;
        if (m_autoDeleteTrials) {
            locker.unlock();
            disable(); // sets to Status::Disabled
            locker.relock();
        }
        setProgress(360);
        // reset the stopAt flag to maximum
        setStopAt(m_inputs->general(GENERAL_ATTR_STOPAT).toInt());
    } else { // all or some trials are paused
        updateProgressValue();
        m_expStatus = Status::Paused; // exp is still good for another step
    }
    setPauseAt(m_stopAt); // reset the pauseAt flag to maximum
    emit (statusChanged(m_expStatus));
}

void Experiment::toggle()
{
    if (m_expStatus == Status::Running) {
        pause();
    } else if (m_expStatus == Status::Paused || m_expStatus == Status::Disabled) {
        play();
    } else if (m_expStatus == Status::Queued) {
        m_mainApp->expMgr()->removeFromQueue(shared_from_this());
    }
}

void Experiment::play()
{
    m_mainApp->expMgr()->play(shared_from_this());
}

void Experiment::playNext()
{
    if (m_expStatus != Status::Paused && m_expStatus != Status::Disabled) {
        return;
    }

    int maxCurrStep = -1;
    for (const auto& trial : m_trials) {
        int currStep = trial.second->step();
        if (currStep > maxCurrStep) maxCurrStep = currStep;
    }
    setPauseAt(maxCurrStep + 1);
    play();
}

Nodes Experiment::cloneCachedNodes(const int trialId)
{
    if (m_clonableNodes.empty()) {
        return Nodes();
    }

    // if it's not the last trial, just take a copy of the nodes
    for (auto const& it : m_trials) {
        if (it.first != trialId && it.second->status() == Status::Disabled) {
            return NodesPrivate::clone(m_clonableNodes);
        }
    }

    // it's the last trial, let's use the cloned nodes
    Nodes nodes = m_clonableNodes;
    Nodes().swap(m_clonableNodes);
    return nodes;
}

AttrsGeneratorPtr Experiment::edgeAttrsGen(bool& ok) const
{
    ok = true;
    if (modelPlugin()->edgeAttrsScope().empty()) {
        return nullptr;
    }
    const QString& cmd = m_inputs->general(GENERAL_ATTR_EDGEATTRS).toQString();
    if (cmd.isEmpty()) {
        qWarning() << GENERAL_ATTR_EDGEATTRS << "must not be empty!";
        ok = false;
        return nullptr;
    }
    QString error;
    auto r = AttrsGenerator::parse(modelPlugin()->edgeAttrsScope(), cmd, error);
    ok = error.isEmpty();
    return r;
}

Nodes Experiment::createNodes() const
{
    const QString& cmd = m_inputs->general(GENERAL_ATTR_NODES).toQString();

    QString error;
    Nodes nodes = NodesPrivate::fromCmd(cmd, m_inputs->modelPlugin()->nodeAttrsScope(), m_graphType, error);
    if (nodes.empty() || !error.isEmpty()) {
        error = QString("unable to create the trials."
                        "The set of nodes could not be created.\n %1 \n"
                        "Experiment: %2").arg(error).arg(m_id);
        qWarning() << error;
    }

    Q_ASSERT_X(nodes.size() <= EVOPLEX_MAX_NODES, "Experiment", "too many nodes to handle!");
    return nodes;
}

bool Experiment::removeOutput(const OutputPtr& output)
{
    if (m_expStatus != Status::Paused) {
        qWarning() << "tried to remove an 'Output' from a running experiment. You should pause it first.";
        return false;
    }

    if (!output->isEmpty()) {
        qWarning() << "tried to remove an 'Output' that seems to be used somewhere. It should be cleaned first.";
        return false;
    }

    auto it = m_outputs.find(output);
    if (it == m_outputs.end()) {
        qWarning() << "tried to remove a non-existent 'Output'.";
        return false;
    }

    m_outputs.erase(it);
    return true;
}

OutputPtr Experiment::searchOutput(const OutputPtr& find)
{
    for (auto const& output : m_outputs) {
        if (output->operator==(find)) {
            return output;
        }
    }
    return nullptr;
}

void Experiment::setExpStatus(Status s)
{
    QMutexLocker locker(&m_mutex);
    m_expStatus = s;
    emit (statusChanged(m_expStatus));
}

void Experiment::setProgress(quint16 p)
{
    m_progress = p;
    emit (progressUpdated(p));
}

} // evoplex
