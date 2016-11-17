/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROCESSESMGR_H
#define PROCESSESMGR_H

#include <QList>
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
    QList<int> add(QList<Simulation*> sims);

    // Add process and tries to play
    int addAndPlay(Simulation* sim);
    QList<int> addAndPlay(QList<Simulation*> sims);

    // Run process
    void play(int processId);
    void play(QList<int> processIds);

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

    inline Simulation* getProcess(int id) { return m_processes.value(id); }

signals:
    void newProcess(int id);
    void killed(int id);

public slots:
    // watcher
    // trigged when a process ends
    void threadFinished();

private:
    int m_threads;
    QList<int> m_runningProcesses;
    QList<int> m_queuedProcesses;
    QList<int> m_processesToKill;
    QMap<int, Simulation*> m_processes;

    // method called by a QtConcurrent::run()
    int runThread(int id);
};

#endif // PROCESSESMGR_H
