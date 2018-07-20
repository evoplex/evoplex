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

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtDebug>

#include "experimentsmgr.h"
#include "experiment.h"
#include "trial.h"

namespace evoplex {

ExperimentsMgr::ExperimentsMgr()
    : m_lastThreadPriority(INT32_MAX),
      m_timerProgress(new QTimer(this))
{
    resetSettingsToDefault();

    m_threads = m_userPrefs.value("settings/threads", m_threads).toInt();
    m_threads = m_threads > QThread::idealThreadCount() ? QThread::idealThreadCount() : m_threads;
    m_threadPool.setMaxThreadCount(m_threads);
    qDebug() << "setting the max number of threads to" << m_threads;

    m_timerProgress->setSingleShot(true);
    connect(m_timerProgress, SIGNAL(timeout()), SLOT(updateProgressValues()));
}

ExperimentsMgr::~ExperimentsMgr()
{
    m_threadPool.clear();
    m_threadPool.waitForDone();
    delete m_timerProgress;
}

void ExperimentsMgr::resetSettingsToDefault()
{
    m_threads = QThread::idealThreadCount();
}

void ExperimentsMgr::updateProgressValues()
{
    if (!m_running.empty()) {
        for (auto const& exp : m_running) {
            exp->updateProgressValue();
        }
        m_timerProgress->start(500);
        emit (progressUpdated());
    }
}

void ExperimentsMgr::play(ExperimentPtr exp)
{
    QtConcurrent::run(this, &ExperimentsMgr::_play, exp);
    m_timerProgress->start(500); // every half a second, check progress
}

void ExperimentsMgr::_play(ExperimentPtr exp)
{
    QMutexLocker locker(&m_mutex);

    if (exp->expStatus() == Status::Invalid ||
            exp->expStatus() == Status::Running ||
            exp->expStatus() == Status::Finished) {
        return;
    }

    if (exp->expStatus() == Status::Disabled && !exp->reset()) {
        return; // something went wrong while initializing the experimnt
    }

    if (exp->expStatus() != Status::Queued) {
        exp->setExpStatus(Status::Queued);
        m_idle.remove(exp);
        m_queued.remove(exp);
        m_queued.emplace_back(exp);
    }

    // iterate by id to maintain id order
    for (quint16 id = 0; id < exp->trials().size(); ++id) {
        Trial* trial = exp->trials().at(id);
        if (trial->status() != Status::Disabled) {
            trial->m_status = Status::Queued;
        }
        m_queuedTrials.emplace_back(trial);
    }

    locker.unlock();
    processQueue();
}

void ExperimentsMgr::trialFinished(Trial* trial)
{
    QMutexLocker locker(&m_mutex);

    m_runningTrials.remove(trial);
    if (isTheLastTrial(trial)) {
        ExperimentPtr exp = trial->m_exp;
        locker.unlock();
        exp->expFinished();
        locker.relock();

        m_running.remove(exp);
        emit (progressUpdated());

        if (exp->expStatus() != Status::Invalid &&
                exp->expStatus() != Status::Disabled) {
            m_idle.emplace_back(exp);
        }

        if (m_running.empty() && m_queued.empty()) {
            m_lastThreadPriority = INT32_MAX; // reset priority
        }
    }

    locker.unlock();
    processQueue();
}

bool ExperimentsMgr::isTheLastTrial(const Trial* trial)
{
    for (auto const& t : m_runningTrials) {
        if (t->m_exp == trial->m_exp)
            return false;
    }
    for (auto const& t : m_queuedTrials) {
        if (t->m_exp == trial->m_exp)
            return false;
    }
    return true;
}

void ExperimentsMgr::processQueue()
{
    QMutexLocker locker(&m_mutex);
    if (static_cast<int>(m_runningTrials.size()) < m_threads) {
        Trial* trial = nullptr;
        while (!trial && !m_queuedTrials.empty()) {
            trial = m_queuedTrials.front();
            m_queuedTrials.pop_front();
        }

        if (!trial) {
            return;
        }

        ExperimentPtr exp = trial->m_exp.toStrongRef();

        // checks if we really need to run this trial
        if (!exp || exp->expStatus() == Status::Invalid ||
                exp->expStatus() == Status::Finished ||
                exp->pauseAt() < 0) { // is paused
            locker.unlock();
            processQueue();
            return;
        }

        // checks if this is the first time we run this experiment
        if (exp->expStatus() != Status::Running) {
            exp->setExpStatus(Status::Running);
            m_running.emplace_back(exp);
            m_queued.remove(exp);
        }

        m_runningTrials.emplace_back(trial);
        // play in the same order of insertion
        m_threadPool.start(trial, m_lastThreadPriority--);

        locker.unlock();
        processQueue();
    }
}

void ExperimentsMgr::removeFromQueue(const ExperimentPtr& exp)
{
    QMutexLocker locker(&m_mutex);
    if (exp->expStatus() == Status::Queued) {
        m_queued.remove(exp);
        exp->setExpStatus(Status::Paused);
    }
}

void ExperimentsMgr::removeFromIdle(const ExperimentPtr& exp)
{
    QMutexLocker locker(&m_mutex);
    m_idle.remove(exp);
}

void ExperimentsMgr::clearQueue()
{
    QMutexLocker locker(&m_mutex);
    for (auto const& exp : m_queued) {
        exp->pause();
        exp->setExpStatus(Status::Paused);
    }
    m_queued.clear();
}

void ExperimentsMgr::setMaxThreadCount(int newValue, QString* error)
{
    if (m_threads == newValue) {
        return;
    }

    if (newValue < 1 || newValue > QThread::idealThreadCount()) {
        QString e = QString("The number of threads is invalid! It should be"
                " greater than 0 and less than %1.\nCurrent: %2; Tried: %3")
                .arg(QThread::idealThreadCount()).arg(m_threads).arg(newValue);
        if (error) *error = e;
        qWarning() << e;
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (!m_running.empty() || !m_queued.empty()) {
        QString e("Cannot set the number of threads while running experiments."
                  " Please, pause all your experiments and try again.");
        if (error) *error = e;
        qWarning() << e;
        return;
    }

    m_threadPool.setMaxThreadCount(newValue);
    if (newValue != m_threadPool.maxThreadCount()) {
        QString e("Could not set the number of threads to %1.\n"
                  "Assigning the maximum value available: %2.");
        if (error) *error = e;
        qWarning() << e;
    }

    qDebug() << "setting the max number of threads from"
             << m_threads << "to" << newValue;

    m_threads = newValue;
    m_userPrefs.setValue("settings/threads", m_threads);
}

} // evoplex
