/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QtDebug>

#include "core/trialsmgr.h"

TrialsMgr::TrialsMgr()
    : m_threads(QThread::idealThreadCount())
    , m_lastTrialId(-1)
{
}

TrialsMgr::~TrialsMgr()
{
    killAll();
}

int TrialsMgr::newTrial(int expId, int projId, AbstractModel *modelObj, int stopAt)
{
    ++m_lastTrialId;
    m_trials.insert(m_lastTrialId, new Trial(m_lastTrialId, expId, projId, modelObj, stopAt));
    emit (added(m_lastTrialId));
    return m_lastTrialId;
}

void TrialsMgr::play(int trialId)
{
    if (m_runningProcesses.contains(trialId)
            || m_queuedProcesses.contains(trialId)) {
        return;
    } else if (!m_trials.contains(trialId)) {
        qWarning() << "[Processes] tried to play an nonexistent process:" << trialId;
        return;
    }

    if (m_runningProcesses.size() < m_threads) {
        m_runningProcesses.append(trialId);

        QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);
        connect(watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
        watcher->setFuture(QtConcurrent::run(this, &TrialsMgr::runThread, trialId));

        m_queuedProcesses.removeAt(trialId);
    } else {
        m_queuedProcesses.append(trialId);
    }
}

void TrialsMgr::stop(int trialId)
{
    m_trials.value(trialId)->stop();
    // make sure it is/will play
    // so, we know that this STOP order will be processed asap
    play(trialId);
}

int TrialsMgr::runThread(int id)
{
    Trial* sim = m_trials.value(id);
    sim->processSteps();
    return id;
}

// watcher
void TrialsMgr::threadFinished()
{
    QFutureWatcher<int>* w = reinterpret_cast<QFutureWatcher<int>*>(sender());
    int processId = w->result();
    m_runningProcesses.removeOne(processId);
    delete w;

    // marked to kill?
    if (m_trialsToKill.contains(processId)) {
        kill(processId);
    }

    // call next process in the queue
    if (!m_queuedProcesses.isEmpty()) {
        play(m_queuedProcesses.first());
    }
}

void TrialsMgr::setNumThreads(int threads)
{
    if (m_threads == threads) {
        return;
    }

    const int p = qAbs(threads - m_threads);
    int old = m_threads;
    m_threads = threads;

    if (threads > old) {
        for (int i = 0; i < p && !m_queuedProcesses.isEmpty(); ++i) {
            int id = m_queuedProcesses.takeFirst();
            play(id);
        }
    } else if (threads < old) {
        for (int i = 0; i < p && !m_runningProcesses.isEmpty(); ++i) {
            int id = m_runningProcesses.takeFirst();
            pause(id);
            m_queuedProcesses.push_front(id);
        }
    }
}

void TrialsMgr::kill(int trialId)
{
    if (m_runningProcesses.contains(trialId)) {
        m_trialsToKill.append(trialId);
    } else {
        m_trialsToKill.removeAt(trialId);
        delete m_trials.take(trialId);
        emit (killed(trialId));
    }
    m_queuedProcesses.removeAt(trialId); // just in case...
}

void TrialsMgr::killAll()
{
    QList<int> ids = m_trials.keys();
    foreach (int id, ids) {
        kill(id);
    }
}
