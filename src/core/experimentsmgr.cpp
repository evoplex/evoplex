/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtDebug>

#include "core/experimentsmgr.h"
#include "core/experiment.h"

ExperimentsMgr::ExperimentsMgr()
    : m_threads(QThread::idealThreadCount())
{
}

ExperimentsMgr::~ExperimentsMgr()
{
    killAll();
}

void ExperimentsMgr::run(Experiment* exp)
{
    if (m_running.contains(exp) || m_queued.contains(exp)) {
        return;
    }

    if (m_running.size() < m_threads) {
        m_running.push_back(exp);

        QVector<int> trialIds;
        trialIds.reserve(exp->getNumTrials());
        for (int id = 0; id < exp->getNumTrials(); ++id)
            trialIds.push_back(id);

        QFutureWatcher<void>* fw = new QFutureWatcher<void>(this);
        connect(fw, &QFutureWatcher<void>::finished, [this, exp]() { exp->finished(); });
        fw->setFuture(QtConcurrent::map(trialIds, [this, exp](int trialId) { exp->processTrial(trialId); }));

        m_queued.removeOne(exp);
    } else {
        m_queued.push_back(exp);
    }
}

void ExperimentsMgr::finished(Experiment* exp)
{
    m_running.removeOne(exp);
    QFutureWatcher<void>* fw = reinterpret_cast<QFutureWatcher<void>*>(sender());
    delete fw;
    fw = nullptr;

    if (m_toKill.contains(exp)) {
        //kill(processId);
    }

    // call next process in the queue
    if (!m_queued.isEmpty()) {
        run(m_queued.first());
    }
}

void ExperimentsMgr::setNumThreads(int threads)
{
    if (m_threads == threads) {
        return;
    }

    const int p = qAbs(threads - m_threads);
    int old = m_threads;
    m_threads = threads;

    if (threads > old) {
        for (int i = 0; i < p && !m_queued.isEmpty(); ++i) {
            run(m_queued.takeFirst());
        }
    } else if (threads < old) {
        for (int i = 0; i < p && !m_running.isEmpty(); ++i) {
            Experiment* exp = m_running.takeFirst();
            run(exp);
            m_queued.push_front(exp);
        }
    }
}

void ExperimentsMgr::kill(Experiment* exp)
{
/*
    if (m_running.contains(exp)) {
        m_toKill.push_back(exp);
    } else {
        m_toKill.removeOne(exp);
        delete m_trials.take(trialId);
        emit (killed(trialId));
    }
    m_queued.removeAt(trialId); // just in case...
*/
}

void ExperimentsMgr::killAll()
{
    foreach (Experiment* exp, m_toKill) {
        kill(exp);
    }
}
