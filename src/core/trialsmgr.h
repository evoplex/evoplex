/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TRIALSMGR_H
#define TRIALSMGR_H

#include <QVector>
#include <QMap>
#include <QObject>

#include "core/trial.h"

class TrialsMgr: public QObject
{
    Q_OBJECT

public:
    explicit TrialsMgr();
    ~TrialsMgr();

    // Create and add a new Trial to the queue.
    // We assume that all parameters are valid at this point.
    int newTrial(int expId, int projId, AbstractModel* modelObj, int stopAt);

    // Run a trial
    void play(int trialId);

    // Pauses a trial asap
    inline void pause(int trialId) { m_trials.value(trialId)->pause(); }
    // Pauses a trial at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    inline void pauseAt(int trialId, int stepToPause) { m_trials.value(trialId)->pauseAt(stepToPause); }

    // Stops trial asap
    void stop(int trialId);
    // Stops trial at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    inline void stopAt(int trialId, int stepToStop) { m_trials.value(trialId)->stopAt(stepToStop); }

    // Kill trial
    void kill(int trialId);

    // Kill all trials
    void killAll();

    // as we are changing the number of threads available,
    // we have to walk through all the processes
    void setNumThreads(int threads);

    inline Trial* getTrial(int id) { return m_trials.value(id, NULL); }

signals:
    void added(int trialId);
    void killed(int id);

public slots:
    // watcher
    // trigged when a process ends
    void threadFinished();

private:
    int m_threads;

    int m_lastTrialId;
    QHash<int, Trial*> m_trials;

    QVector<int> m_runningProcesses;
    QVector<int> m_queuedProcesses;
    QVector<int> m_trialsToKill;

    // method called by a QtConcurrent::run()
    int runThread(int id);
};

#endif // TRIALSMGR_H
