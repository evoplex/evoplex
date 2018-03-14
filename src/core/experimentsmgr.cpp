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

ExperimentsMgr::ExperimentsMgr()
    : m_timerProgress(new QTimer(this))
    , m_timerDestroy(new QTimer(this))
{
    resetSettingsToDefault();

    m_threads = m_userPrefs.value("settings/threads", m_threads).toInt();
    m_threads = m_threads > QThread::idealThreadCount() ? QThread::idealThreadCount() : m_threads;
    m_threadPool.setMaxThreadCount(m_threads);
    qDebug() << "[ExperimentsMgr]: setting the max number of threads to" << m_threads;

    m_timerDestroy->setSingleShot(true);
    m_timerProgress->setSingleShot(true);
    connect(m_timerDestroy, SIGNAL(timeout()), SLOT(destroyExperiments()));
    connect(m_timerProgress, SIGNAL(timeout()), SLOT(updateProgressValues()));

    // call next process in the queue
    connect(this, &ExperimentsMgr::expFinished, this,
            [this]() { if (!m_queued.empty()) { play(m_queued.front()); } },
            Qt::QueuedConnection);
}

ExperimentsMgr::~ExperimentsMgr()
{
    m_threadPool.clear();
    m_threadPool.waitForDone();
    delete m_timerProgress;
    delete m_timerDestroy;
}

void ExperimentsMgr::resetSettingsToDefault()
{
    m_threads = QThread::idealThreadCount();
}

void ExperimentsMgr::updateProgressValues()
{
    if (m_running.size()) {
        for (Experiment* exp : m_running) {
            if (exp) exp->updateProgressValue();
        }
        m_timerProgress->start(500);
    }
}

void ExperimentsMgr::destroyExperiments()
{
    std::list<Experiment*>::iterator it = m_toDestroy.begin();
    while (it != m_toDestroy.end()) {
        Experiment* exp = (*it);
        if (exp->expStatus() == Experiment::INVALID) {
            exp->deleteLater();
            it = m_toDestroy.erase(it);
            continue;
        } else if (exp->expStatus() == Experiment::RUNNING) {
            exp->pause();
        } else {
            m_queued.remove(exp);
            exp->setExpStatus(Experiment::INVALID);
            emit (statusChanged(exp));
        }
        ++it;
    }

    if (!m_toDestroy.empty()) {
        m_timerDestroy->start(500);
    }
}

void ExperimentsMgr::destroy(Experiment* exp)
{
    m_toDestroy.emplace_back(exp);
    destroyExperiments();
}

void ExperimentsMgr::play(Experiment* exp)
{
    QMutexLocker locker(&m_mutex);

    if (exp->expStatus() != Experiment::READY
            && exp->expStatus() != Experiment::QUEUED) {
        return;
    }

    if (m_threadPool.activeThreadCount() < m_threads) {
        exp->setExpStatus(Experiment::RUNNING);
        emit (statusChanged(exp));
        m_queued.remove(exp);

        m_running.emplace_back(exp);
        m_timerProgress->start(500); // every half a second, check progress

        for (int trialId = 0; trialId < exp->numTrials(); ++trialId) {
            m_runningTrials.emplace_back(std::make_pair(exp->id(), trialId));
            m_threadPool.start(new TrialRunnable(this, exp, trialId),
                               -1 * m_runningTrials.size()); // play in the same order of insertion
        }
    } else if (exp->expStatus() != Experiment::QUEUED) {
        exp->setExpStatus(Experiment::QUEUED);
        emit (statusChanged(exp));
        m_queued.emplace_back(exp);
    }
}

void ExperimentsMgr::finished(Experiment* exp, const int trialId)
{
    QMutexLocker locker(&m_mutex);

    m_runningTrials.remove(std::make_pair(exp->id(), trialId));
    for (auto& expTrial : m_runningTrials) {
        if (expTrial.first == exp->id()) {
            return;
        }
    }

    m_running.remove(exp);

    if (std::find(m_toDestroy.begin(), m_toDestroy.end(), exp) != m_toDestroy.end()) {
        exp->setExpStatus(Experiment::INVALID);
    } else if(exp->expStatus() != Experiment::INVALID) {
        for (auto& trial : exp->trials()) {
            if (trial.second->status() != Experiment::FINISHED) {
                exp->setExpStatus(Experiment::READY);
                exp->setPauseAt(EVOPLEX_MAX_STEPS); // reset the pauseAt flag to maximum
                m_idle.emplace_back(exp);
                break;
            }
        }

        if (exp->expStatus() != Experiment::READY) {
            exp->setExpStatus(Experiment::FINISHED);
            if (exp->autoDeleteTrials()) {
                exp->deleteTrials();
            } else {
                m_idle.emplace_back(exp);
            }
        }
    }

    emit (statusChanged(exp));
    emit (expFinished());
}

void ExperimentsMgr::removeFromQueue(Experiment* exp)
{
    QMutexLocker locker(&m_mutex);

    if (exp->expStatus() == Experiment::QUEUED) {
        m_queued.remove(exp);
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
}

void ExperimentsMgr::clearQueue()
{
    QMutexLocker locker(&m_mutex);

    for (Experiment* exp : m_queued) {
        exp->pause();
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
    m_queued.clear();
}

void ExperimentsMgr::clearIdle()
{
    QMutexLocker locker(&m_mutex);

    for (Experiment* exp : m_idle) {
        exp->deleteTrials();
    }
    m_idle.clear();
}

void ExperimentsMgr::setMaxThreadCount(const int newValue)
{
    if (m_threads == newValue) {
        return;
    } else if (newValue < 1 || newValue > QThread::idealThreadCount()) {
        QString err = QString("[ExperimentMgr]: invalid number of threads!"
                    "It should be an integer greater than 0 and less than %1.\n"
                    "Current: %2; Tried: %3").arg(QThread::idealThreadCount())
                    .arg(m_threads).arg(newValue);
        qWarning() << err;
        return;
    }

    const int previous = m_threads;
    const int diff = qAbs(newValue - previous);
    m_threads = newValue;

    if (newValue > previous) {
        for (int n = 0; n < diff && !m_queued.empty(); ++n) {
            play(m_queued.front());
        }
    } else if (newValue < previous) {
        for (int n = 0; n < diff && !m_running.empty(); ++n) {
            m_running.back()->pause();
        }
    }

    m_userPrefs.setValue("settings/threads", m_threads);
    m_threadPool.setMaxThreadCount(m_threads);
    qDebug() << "[ExperimentsMgr] setting the max number of thread from"
             << previous << "to" << newValue;
}

/********************************/

TrialRunnable::TrialRunnable(ExperimentsMgr* expMgr, Experiment* exp, int trialId)
    : expMgr(expMgr)
    , m_exp(exp)
    , m_trialId(trialId)
{
}

void TrialRunnable::run()
{
    m_exp->processTrial(m_trialId);
    expMgr->finished(m_exp, m_trialId);
}

}
