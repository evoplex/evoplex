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
