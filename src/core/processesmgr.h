/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROCESSESMGR_H
#define PROCESSESMGR_H

#include <QVector>
#include <QMap>
#include <QObject>

#include "core/simulation.h"

class ProcessesMgr: public QObject
{
    Q_OBJECT

public:
    explicit ProcessesMgr();
    ~ProcessesMgr();

    // Returns the processId or 0 if something went wrong
    int add(Simulation* sim);
    QVector<int> add(QVector<Simulation*> sims);

    // Add process and tries to play
    int addAndPlay(Simulation* sim);
    QVector<int> addAndPlay(QVector<Simulation*> sims);

    // Run process
    void play(int processId);
    void play(QVector<int> processIds);

    // Pauses process asap
    void pause(int processId);

    // Pauses process at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    void pauseAt(int processId, quint64 stepToPause);

    // Stops process asap
    void stop(int processId);

    // Stops process at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    void stopAt(int processId, quint64 stepToStop);

    // Kill process
    void kill(int processId);

    // Kill all processes
    void killAll();

    // as we are changing the number of threads available,
    // we have to walk through all the processes
    void setNumThreads(int threads);

    inline Simulation* getProcess(int id) { return m_processes.value(id, NULL); }

signals:
    void newProcess(int id);
    void killed(int id);

public slots:
    // watcher
    // trigged when a process ends
    void threadFinished();

private:
    int m_threads;
    QVector<int> m_runningProcesses;
    QVector<int> m_queuedProcesses;
    QVector<int> m_processesToKill;
    QMap<int, Simulation*> m_processes; // using qmap because to keep the ids in order

    // method called by a QtConcurrent::run()
    int runThread(int id);
};

#endif // PROCESSESMGR_H
