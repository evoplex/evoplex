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
#include "project.h"
#include "utils.h"

namespace evoplex {

Experiment::Experiment(MainApp* mainApp, const int id, ProjectPtr project)
    : m_mainApp(mainApp),
      m_id(id),
      m_project(project),
      m_inputs(nullptr),
      m_graphPlugin(nullptr),
      m_modelPlugin(nullptr),
      m_graphType(GraphType::Invalid),
      m_numTrials(0),
      m_autoDeleteTrials(true),
      m_stopAt(0),
      m_pauseAt(0),
      m_progress(0),
      m_delay(0),
      m_expStatus(Status::Invalid)
{
    Q_ASSERT_X(!m_project.isNull(), "Experiment", "an experiment must belong to a valid project");
}

Experiment::~Experiment()
{
    Q_ASSERT_X(m_expStatus != Status::Running && m_expStatus != Status::Queued,
               "~Experiment", "tried to delete a running experiment");
    deleteTrials();
    m_outputs.clear();
    delete m_inputs;
    m_project.clear();
}

bool Experiment::setInputs(ExpInputs* inputs, QString& error)
{
    QMutexLocker locker(&m_mutex);

    if (m_expStatus == Status::Running || m_expStatus == Status::Queued) {
        error += "Tried to initialize a running experiment.\n"
                 "Please, pause it and try again.";
        qWarning() << error;
        return false;
    }

    m_expStatus = Status::Invalid;
    m_outputs.clear();
    m_filePathPrefix.clear();
    m_fileHeader.clear();
    delete m_inputs;
    m_inputs = inputs;
    locker.unlock();
    deleteTrials();
    locker.relock();

    m_graphPlugin = m_mainApp->graph(m_inputs->general(GENERAL_ATTRIBUTE_GRAPHID).toQString());
    m_modelPlugin = m_mainApp->model(m_inputs->general(GENERAL_ATTRIBUTE_MODELID).toQString());

    // a few asserts for critical things that should never happen!
    Q_ASSERT_X(this == m_project->experiment(m_id),
        "Experiment", "an experiment must be aware of its parent project!");
    Q_ASSERT_X(m_id == inputs->general(GENERAL_ATTRIBUTE_EXPID).toInt(),
        "Experiment", "mismatched experiment id!");
    Q_ASSERT_X(m_graphPlugin && m_modelPlugin,
        "Experiment", "tried to setup an experiment with invalid plugins!");

    m_graphType = _enumFromString<GraphType>(m_inputs->general(GENERAL_ATTRIBUTE_GRAPHTYPE).toQString());
    if (m_graphType == GraphType::Invalid) {
        error += "the graph type is invalid!";
        qWarning() << error;
        emit (statusChanged(m_expStatus));
        return false;
    }

    m_numTrials = m_inputs->general(GENERAL_ATTRIBUTE_TRIALS).toInt();
    if (m_numTrials < 1 || m_numTrials > EVOPLEX_MAX_TRIALS) {
        error += QString("number of trials should be >0 and <%1!").arg(EVOPLEX_MAX_TRIALS);
        qWarning() << error;
        emit (statusChanged(m_expStatus));
        return false;
    }

    m_autoDeleteTrials = m_inputs->general(GENERAL_ATTRIBUTE_AUTODELETE).toBool();
    m_stopAt = m_inputs->general(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    m_progress = 0;

    m_expStatus = Status::Unset;
    emit (statusChanged(m_expStatus));
    emit (restarted());
    return true;
}

void Experiment::init()
{
    Q_ASSERT_X(m_inputs, "Experiment", "tried to init an experiment without inputs");
    Q_ASSERT_X(m_expStatus == Status::Unset, "Experiment", "tried to init an experiment twice");

    if (!m_inputs->fileCaches().empty()) {
        m_filePathPrefix = QString("%1/%2_e%3_t")
                .arg(m_inputs->general(OUTPUT_DIR).toQString())
                .arg(m_project->name())
                .arg(m_id);

        for (const Cache* cache : m_inputs->fileCaches()) {
            Q_ASSERT_X(cache->inputs().size() > 0, "Experiment::init", "a file cache must have inputs");
            m_fileHeader += cache->printableHeader(',', false) + ",";
            m_outputs.insert(cache->output());
        }
        m_fileHeader.chop(1);
        m_fileHeader += "\n";
    }

    // if it comes from playNext(), pauseAt should not be reset
    const int pauseAt = m_pauseAt;
    m_expStatus = Status::Ready;
    reset();
    m_pauseAt = pauseAt;
}

void Experiment::reset()
{
    Q_ASSERT_X(m_expStatus != Status::Unset, "Experiment",
               "the experiment must be initialized first");

    QMutexLocker locker(&m_mutex);

    if (m_expStatus == Status::Running || m_expStatus == Status::Queued) {
        qWarning() << "tried to reset a running experiment. You should pause it first.";
        return;
    }

    m_delay = m_mainApp->defaultStepDelay();
    m_stopAt = m_inputs->general(GENERAL_ATTRIBUTE_STOPAT).toInt();
    m_pauseAt = m_stopAt;
    m_progress = 0;

    locker.unlock();
    deleteTrials();
    locker.relock();

    for (OutputPtr o : m_outputs) {
        o->flushAll();
    }

    m_trials.reserve(static_cast<size_t>(m_numTrials));
    for (quint16 trialId = 0; trialId < m_numTrials; ++trialId) {
        m_trials.insert({trialId, new Trial(trialId, this)});
    }

    m_expStatus = Status::Ready;
    emit (statusChanged(m_expStatus));

    emit (restarted());
}

const Trial* Experiment::trial(quint16 trialId) const
{
    auto it = m_trials.find(trialId);
    if (it == m_trials.end())
        return nullptr;
    return it->second;
}

void Experiment::deleteTrials()
{
    QMutexLocker locker(&m_mutex);

    for (auto& trial : m_trials) {
        delete trial.second;
    }
    m_trials.clear();

    m_clonableNodes.clear();
}

void Experiment::updateProgressValue()
{
    quint16 lastProgress = m_progress;
    if (m_expStatus == Status::Finished) {
        m_progress = 360;
    } else if (m_expStatus == Status::Invalid) {
        m_progress = 0;
    } else if (m_expStatus == Status::Running) {
        float p = 0.f;
        for (auto& trial : m_trials) {
            p += (static_cast<float>(trial.second->step()) / m_pauseAt);
        }
        m_progress = static_cast<quint16>(std::ceil(p * 360.f / m_numTrials));
    }

    if (lastProgress != m_progress) {
        emit (progressUpdated());
    }
}

void Experiment::toggle()
{
    if (m_expStatus == Status::Running) {
        pause();
    } else if (m_expStatus == Status::Ready || m_expStatus == Status::Unset) {
        play();
    } else if (m_expStatus == Status::Queued) {
        m_mainApp->expMgr()->removeFromQueue(this);
    }
}

void Experiment::play()
{
    m_mainApp->expMgr()->play(this);
}

void Experiment::playNext()
{
    if (m_expStatus != Status::Ready && m_expStatus != Status::Unset) {
        return;
    }

    int maxCurrStep = -100;
    for (const auto& trial : m_trials) {
        int currStep = trial.second->step();
        if (currStep > maxCurrStep) maxCurrStep = currStep;
    }
    setPauseAt(maxCurrStep + 1);
    m_mainApp->expMgr()->play(this);
}

Nodes Experiment::cloneCachedNodes(const int trialId)
{
    if (m_clonableNodes.empty()) {
        return Nodes();
    }

    // if it's not the last trial, just take a copy of the nodes
    for (auto const& it : m_trials) {
        if (it.first != trialId && it.second->status() == Status::Unset) {
            return Utils::clone(m_clonableNodes);
        }
    }

    // it's the last trial, let's use the cloned nodes
    Nodes nodes = m_clonableNodes;
    Nodes().swap(m_clonableNodes);
    return nodes;
}

bool Experiment::createNodes(Nodes& nodes) const
{
    const QString& cmd = m_inputs->general(GENERAL_ATTRIBUTE_NODES).toQString();

    QString error;
    nodes = Nodes::fromCmd(cmd, m_modelPlugin->nodeAttrsScope(), m_graphType, error);
    if (nodes.empty() || !error.isEmpty()) {
        error = QString("unable to create the trials."
                         "The set of nodes could not be created.\n %1 \n"
                         "Project: %2 Experiment: %3")
                         .arg(error).arg(m_project->name()).arg(m_id);
        qWarning() << error;
        return false;
    }

    Q_ASSERT_X(nodes.size() <= EVOPLEX_MAX_NODES, "Experiment", "too many nodes to handle!");
    return true;
}

bool Experiment::removeOutput(OutputPtr output)
{
    if (m_expStatus != Status::Ready) {
        qWarning() << "tried to remove an 'Output' from a running experiment. You should pause it first.";
        return false;
    }

    if (!output->isEmpty()) {
        qWarning() << "tried to remove an 'Output' that seems to be used somewhere. It should be cleaned first.";
        return false;
    }

    std::unordered_set<OutputPtr>::iterator it = m_outputs.find(output);
    if (it == m_outputs.end()) {
        qWarning() << "tried to remove a non-existent 'Output'.";
        return false;
    }

    m_outputs.erase(it);
    return true;
}

OutputPtr Experiment::searchOutput(const OutputPtr find)
{
    for (OutputPtr output : m_outputs) {
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

} // evoplex
