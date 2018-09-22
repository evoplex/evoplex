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

#ifndef EXPERIMENTMGR_H
#define EXPERIMENTMGR_H

#include <list>
#include <memory>

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <QSettings>
#include <QThreadPool>

namespace evoplex {

class Trial;
class Experiment;
using ExperimentPtr = std::shared_ptr<Experiment>;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

public:
    explicit ExperimentsMgr();
    ~ExperimentsMgr();

    void resetSettingsToDefault();

    void play(ExperimentPtr exp);

    inline int maxThreadsCount() const { return m_threads; }
    void setMaxThreadCount(const int newValue, QString* error=nullptr);

    // trigged when a Trial ends
    // also runs in a work thread
    void trialFinished(Trial* trial);

    void remove(const ExperimentPtr& exp);
    void removeFromQueue(const ExperimentPtr& exp);
    void removeFromIdle(const ExperimentPtr& exp);

public slots:
    void clearQueue();

signals:
    void progressUpdated();

private slots:
    void updateProgressValues();

private:
    QThreadPool m_threadPool;
    QMutex m_mutex;
    QSettings m_userPrefs;
    int m_threads;
    int m_lastThreadPriority;

    QTimer* m_timerProgress; // update the progress value of all running experiments

    std::list<Trial*> m_runningTrials;
    std::list<Trial*> m_queuedTrials;

    std::list<ExperimentPtr> m_running;
    std::list<ExperimentPtr> m_queued;
    std::list<ExperimentPtr> m_idle;

    void _play(ExperimentPtr exp);

    void processQueue();

    bool isTheLastTrial(const Trial* trial);
};

} // evoplex
#endif // EXPERIMENTMGR_H
