/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
        exp->expFinished(); // might delete all trials
        locker.relock();

        m_running.remove(exp);
        emit (progressUpdated());

        if (exp->expStatus() != Status::Invalid && exp->expStatus() != Status::Disabled) {
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

        Experiment* exp = trial->m_exp.get();

        // checks if we really need to run this trial
        if (!exp || exp->expStatus() == Status::Invalid ||
                exp->expStatus() == Status::Finished ||
                exp->pauseAt() < 0) { // is paused
            locker.unlock();
            trialFinished(trial);
            return;
        }

        // checks if this is the first time we run this experiment
        if (exp->expStatus() != Status::Running) {
            exp->setExpStatus(Status::Running);
            m_running.emplace_back(trial->m_exp);
            m_queued.remove(trial->m_exp);
        }

        m_runningTrials.emplace_back(trial);
        // play in the same order of insertion
        m_threadPool.start(trial, m_lastThreadPriority--);

        locker.unlock();
        processQueue();
    }
}

void ExperimentsMgr::remove(const ExperimentPtr& exp)
{
    removeFromQueue(exp);
    QMutexLocker locker(&m_mutex);
    m_idle.remove(exp);
}

void ExperimentsMgr::removeFromQueue(const ExperimentPtr& exp)
{
    QMutexLocker locker(&m_mutex);
    if (exp->expStatus() == Status::Queued) {
        m_queued.remove(exp);
        auto it = m_queuedTrials.begin();
        while (it != m_queuedTrials.end()) {
            if ((*it)->m_exp == exp) {
                it = m_queuedTrials.erase(it);
            } else {
                ++it;
            }
        }
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
