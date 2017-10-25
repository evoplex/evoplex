/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtDebug>

#include "experimentsmgr.h"
#include "experiment.h"

namespace evoplex {

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
    for (Experiment* exp : m_running) {
        quint16 p = exp->getProgress();
        exp->updateProgressValue();
        if (p != exp->getProgress())
            emit (progressUpdated(exp));
    }
}

void ExperimentsMgr::play(Experiment* exp)
{
    if (exp->getExpStatus() != Experiment::READY
            && exp->getExpStatus() != Experiment::QUEUED) {
        return;
    }

    if (m_running.size() < m_threads) {
        exp->setExpStatus(Experiment::RUNNING);
        emit (statusChanged(exp));
        m_queued.remove(exp);

        m_running.push_back(exp);
        if (!m_timer->isActive())
            m_timer->start(500); // every half a second, check progress

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
    m_running.remove(exp);
    if (m_running.empty()) {
        m_timer->stop();
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

    if (exp->getExpStatus() == Experiment::FINISHED && exp->getAutoDelete()) {
        exp->deleteTrials();
    }

    // call next process in the queue
    if (!m_queued.empty()) {
        play(m_queued.front());
    }
}

void ExperimentsMgr::removeFromQueue(Experiment* exp)
{
    if (exp->getExpStatus() == Experiment::QUEUED) {
        m_queued.remove(exp);
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
}

void ExperimentsMgr::clearQueue()
{
    foreach (Experiment* exp, m_queued) {
        exp->pause();
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
    m_queued.clear();
}

void ExperimentsMgr::setMaxThreadCount(const int newValue)
{
    if (m_threads == newValue || newValue < 1) {
        return;
    }

    const int previous = m_threads;
    m_threads = newValue;

    if (newValue > previous) {
        for (Experiment* exp : m_queued) {
            play(exp);
        }
    } else if (newValue < previous) {
        const int diff = previous - newValue;
        int n = 0;
        for (Experiment* exp : m_running) {
            if (n < diff) break;
            exp->pause();
        }
    }

    QThreadPool::globalInstance()->setMaxThreadCount(m_threads);
    qDebug() << "[ExperimentsMgr] setting the max number of thread from"
             << previous << "to" << newValue;
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
}
