/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFutureWatcher>
#include <QList>
#include <QtConcurrent/QtConcurrentRun>
#include <QtDebug>

#include "core/processesmgr.h"

ProcessesMgr::ProcessesMgr()
    : m_threads(QThread::idealThreadCount())
{
}

ProcessesMgr::~ProcessesMgr()
{
    killAll();
}

int ProcessesMgr::add(Simulation* sim)
{
    int key = m_processes.key(sim, -1);
    if (key == -1) {
        key = m_processes.isEmpty() ? 0 : m_processes.lastKey() + 1;
        m_processes.insert(key, sim);
    }
    return key;
}

QList<int> ProcessesMgr::add(QList<Simulation*> sims)
{
    QList<int> ids;
    int id = m_processes.lastKey();
    foreach (Simulation* sim, sims) {
        ids.append(id);
        m_processes.insert(id, sim);
        ++id;
    }
    return ids;
}

int ProcessesMgr::addAndPlay(Simulation* sim)
{
    int key = add(sim);
    play(key);
    return key;
}

QList<int> ProcessesMgr::addAndPlay(QList<Simulation*> sims)
{
   QList<int> keys = add(sims);
   play(keys);
   return keys;
}

void ProcessesMgr::play(int id)
{
    if (m_runningProcesses.contains(id)
            && m_queuedProcesses.contains(id)) {
        return;
    } else if (!m_processes.contains(id)) {
        qWarning() << "[Processes] tried to play an nonexistent process:" << id;
        return;
    }

    if (m_runningProcesses.size() < m_threads) {
        m_runningProcesses.append(id);

        QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);
        connect(watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
        watcher->setFuture(QtConcurrent::run(this, &ProcessesMgr::runThread, id));

        m_queuedProcesses.removeAt(id);
    } else {
        m_queuedProcesses.append(id);
    }
}

void ProcessesMgr::play(QList<int> ids)
{
    foreach (int id, ids) {
        play(id);
    }
}

void ProcessesMgr::pause(int id)
{
    if (!m_runningProcesses.contains(id) || !m_processes.contains(id)) {
        return;
    }
    m_processes.value(id)->pause();
}

void ProcessesMgr::pauseAt(int id, quint64 step)
{
    if (!m_runningProcesses.contains(id) || !m_processes.contains(id)) {
        return;
    }
    m_processes.value(id)->pauseAt(step);
}

void ProcessesMgr::stop(int id)
{
    if (!m_runningProcesses.contains(id) || !m_processes.contains(id)) {
        return;
    }
    m_processes.value(id)->stop();
}

void ProcessesMgr::stopAt(int id, quint64 step)
{
    if (!m_runningProcesses.contains(id) || !m_processes.contains(id)) {
        return;
    }
    m_processes.value(id)->stopAt(step);
}

int ProcessesMgr::runThread(int id)
{
    Simulation* sim = m_processes.value(id);
    sim->processSteps();
    return id;
}

// watcher
void ProcessesMgr::threadFinished()
{
    QFutureWatcher<int>* w = reinterpret_cast<QFutureWatcher<int>*>(sender());
    int id = w->result();
    m_runningProcesses.removeAt(id);

    // marked to kill?
    if (m_processesToKill.contains(id)) {
        kill(id);
    }

    // call next process in the queue
    if (!m_queuedProcesses.isEmpty()) {
        play(m_queuedProcesses.first());
    }
}

void ProcessesMgr::setNumThreads(int threads)
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

void ProcessesMgr::kill(int id)
{
    if (m_runningProcesses.contains(id)) {
        m_processesToKill.append(id);
    } else {
        m_processesToKill.removeAt(id);
        delete m_processes.take(id);
        emit (killed(id));
    }
    m_queuedProcesses.removeAt(id); // just in case...
}

void ProcessesMgr::killAll()
{
    QList<int> ids = m_processes.keys();
    foreach (int id, ids) {
        kill(id);
    }
}
