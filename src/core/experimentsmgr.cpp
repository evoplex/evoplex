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
    QThreadPool::globalInstance()->setMaxThreadCount(m_threads);
    qDebug() << "[ExperimentsMgr]: setting the max number of threads to" << m_threads;

    m_timerDestroy->setSingleShot(true);
    connect(m_timerDestroy, SIGNAL(timeout()), this, SLOT(destroyExperiments()));
    connect(m_timerProgress, SIGNAL(timeout()), this, SLOT(updateProgressValues()));
}

ExperimentsMgr::~ExperimentsMgr()
{
    delete m_timerProgress;
    delete m_timerDestroy;
}

void ExperimentsMgr::resetSettingsToDefault()
{
    m_threads = QThread::idealThreadCount();
}

void ExperimentsMgr::updateProgressValues()
{
    for (Experiment* exp : m_running) {
        quint16 p = exp->progress();
        exp->updateProgressValue();
        if (p != exp->progress())
            emit (progressUpdated(exp));
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
    m_timerDestroy->start(500);
}

void ExperimentsMgr::play(Experiment* exp)
{
    QMutexLocker locker(&m_mutex);

    if (exp->expStatus() != Experiment::READY
            && exp->expStatus() != Experiment::QUEUED) {
        return;
    }

    if (m_running.size() < m_threads) {
        exp->setExpStatus(Experiment::RUNNING);
        emit (statusChanged(exp));
        m_queued.remove(exp);

        m_running.emplace_back(exp);
        if (!m_timerProgress->isActive())
            m_timerProgress->start(500); // every half a second, check progress

        // both the QVector and the QFutureWatcher must live longer
        // so, they must be pointers.
        QVector<int>* trialIds = new QVector<int>;
        trialIds->reserve(exp->numTrials());
        for (int id = 0; id < exp->numTrials(); ++id)
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
    } else if (exp->expStatus() != Experiment::QUEUED) {
        exp->setExpStatus(Experiment::QUEUED);
        emit (statusChanged(exp));
        m_queued.emplace_back(exp);
    }
}

void ExperimentsMgr::finished(Experiment* exp)
{
    m_running.remove(exp);
    if (m_running.empty()) {
        m_timerProgress->stop();
    }

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

    // call next process in the queue
    if (!m_queued.empty()) {
        play(m_queued.front());
    }
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

    QMutexLocker locker(&m_mutex);

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
    QThreadPool::globalInstance()->setMaxThreadCount(m_threads);
    qDebug() << "[ExperimentsMgr] setting the max number of thread from"
             << previous << "to" << newValue;
}

}
