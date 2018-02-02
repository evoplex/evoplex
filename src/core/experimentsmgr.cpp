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
    : m_timer(new QTimer(this))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateProgressValues()));

    QSettings s;
    m_threads = s.value("settings/threads", QThread::idealThreadCount()).toInt();
    m_threads = m_threads > QThread::idealThreadCount() ? QThread::idealThreadCount() : m_threads;
}

ExperimentsMgr::~ExperimentsMgr()
{
    delete m_timer;
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

void ExperimentsMgr::play(Experiment* exp)
{
    if (exp->expStatus() != Experiment::READY
            && exp->expStatus() != Experiment::QUEUED) {
        return;
    }

    if (m_running.size() < m_threads) {
        exp->setExpStatus(Experiment::RUNNING);
        emit (statusChanged(exp));
        m_queued.remove(exp);

        m_running.emplace_back(exp);
        if (!m_timer->isActive())
            m_timer->start(500); // every half a second, check progress

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
        m_timer->stop();
    }

    exp->setPauseAt(EVOPLEX_MAX_STEPS); // reset the pauseAt flag to maximum

    if(exp->expStatus() != Experiment::INVALID) {
        exp->setExpStatus(Experiment::FINISHED);
        for (auto& trial : exp->trials()) {
            if (trial.second->status() != Experiment::FINISHED) {
                exp->setExpStatus(Experiment::READY);
                break;
            }
        }
    }
    emit (statusChanged(exp));

    if (exp->expStatus() == Experiment::FINISHED && exp->autoDelete()) {
        exp->deleteTrials();
    } else {
        m_idle.emplace_back(exp);
    }

    // call next process in the queue
    if (!m_queued.empty()) {
        play(m_queued.front());
    }
}

void ExperimentsMgr::removeFromQueue(Experiment* exp)
{
    if (exp->expStatus() == Experiment::QUEUED) {
        m_queued.remove(exp);
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
}

void ExperimentsMgr::clearQueue()
{
    for (Experiment* exp : m_queued) {
        exp->pause();
        exp->setExpStatus(Experiment::READY);
        emit (statusChanged(exp));
    }
    m_queued.clear();
}

void ExperimentsMgr::clearIdle()
{
    for (Experiment* exp : m_idle) {
        exp->deleteTrials();
    }
    m_idle.clear();
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

}
