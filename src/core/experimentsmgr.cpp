/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtDebug>

#include "core/experimentsmgr.h"
#include "core/experiment.h"

ExperimentsMgr::ExperimentsMgr(int threads)
    : m_threads(threads)
    , m_timer(new QTimer(this))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateProgressValues()));
}

ExperimentsMgr::~ExperimentsMgr()
{
    killAll();
}

void ExperimentsMgr::updateProgressValues()
{
    for (int i = 0; i < m_running.size(); ++i) {
        quint16 p = m_running.at(i)->getProgress();
        m_running.at(i)->updateProgressValue();
        if (p != m_running.at(i)->getProgress())
            emit (progressUpdated(m_running.at(i)));
    }
}

void ExperimentsMgr::run(Experiment* exp)
{
    if (exp->getExpStatus() != Experiment::READY
            && exp->getExpStatus() != Experiment::QUEUED) {
        return;
    }

    if (m_running.size() < m_threads) {
        exp->setExpStatus(Experiment::RUNNING);
        emit (statusChanged(exp));
        m_queued.removeOne(exp);

        m_running.push_back(exp);
        m_timer->start(500); // every half a second

        // both the QVector and the QFutureWatcher must live longer
        // so, they must be pointers.
        QVector<int>* trialIds = new QVector<int>;
        trialIds->reserve(exp->getNumTrials());
        for (int id = 0; id < exp->getNumTrials(); ++id)
            trialIds->push_back(id);

        QFutureWatcher<void>* fw = new QFutureWatcher<void>(this);
        connect(fw, &QFutureWatcher<void>::finished,
                [this, fw, trialIds, exp]() {
                    finished(exp);
                    fw->deleteLater();
                    trialIds->clear();
                    trialIds->squeeze();
                    delete trialIds;
                }
        );

        fw->setFuture(QtConcurrent::map(trialIds->begin(), trialIds->end(),
                [this, exp](int& trialId) { exp->processTrial(trialId); }));
    } else if (exp->getExpStatus() != Experiment::QUEUED) {
        exp->setExpStatus(Experiment::QUEUED);
        emit (statusChanged(exp));
        m_queued.push_back(exp);
    }
}

void ExperimentsMgr::finished(Experiment* exp)
{
    m_running.removeOne(exp);
    if (m_running.isEmpty()) {
        m_timer->stop();
    }

    if (m_toKill.contains(exp)) {
        //kill(processId);
    }

    // call next process in the queue
    if (!m_queued.isEmpty()) {
        run(m_queued.first());
    }

    exp->pauseAt(EVOPLEX_MAX_STEPS); // reset the pauseAt flag to maximum

    if(exp->getExpStatus() != Experiment::INVALID) {
        exp->setExpStatus(Experiment::FINISHED);
        const QHash<int, Experiment::Trial>& trials = exp->getTrials();
        QHash<int, Experiment::Trial>::const_iterator it = trials.begin();
        while (it != trials.end()) {
            if (it.value().status != Experiment::FINISHED) {
                exp->setExpStatus(Experiment::READY);
                break;
            }
            ++it;
        }
    }

    emit (statusChanged(exp));
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
