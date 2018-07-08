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

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <QSettings>
#include <QThreadPool>
#include <list>

namespace evoplex {

class Experiment;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

    friend class TrialRunnable;

public:
    explicit ExperimentsMgr();
    ~ExperimentsMgr();

    void resetSettingsToDefault();

    void play(Experiment* exp);

    inline int maxThreadsCount() const { return m_threads; }
    void setMaxThreadCount(const int newValue);

signals:
    void expFinished();

public slots:
    void clearQueue();
    void clearIdle();
    void removeFromQueue(Experiment* exp);
    void destroy(Experiment* exp);

private slots:
    void updateProgressValues();
    void destroyExperiments();

private:
    QThreadPool m_threadPool;
    QMutex m_mutex;
    QSettings m_userPrefs;
    int m_threads;

    QTimer* m_timerProgress; // update the progress value of all running experiments
    QTimer* m_timerDestroy;

    std::list<std::pair<int,int>> m_runningTrials;
    std::list<Experiment*> m_running;
    std::list<Experiment*> m_queued;
    std::list<Experiment*> m_idle;
    std::list<Experiment*> m_toDestroy;

    // trigged when an experiment ends (TrialRunnable)
    // also runs in a work thread
    void finished(Experiment* exp, const int trialId);
};

/********************************/

class TrialRunnable: public QRunnable
{
public:
    TrialRunnable(ExperimentsMgr* expMgr, Experiment* exp, quint16 trialId);
    void run();

private:
    ExperimentsMgr* expMgr;
    Experiment* m_exp;
    const quint16 m_trialId;
};

}

#endif // EXPERIMENTMGR_H
