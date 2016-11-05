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
    quint16 add(Simulation* sim);
    QList<quint16> add(QList<Simulation*> sims);

    // Add process and tries to play
    quint16 addAndPlay(Simulation* sim);
    QList<quint16> addAndPlay(QList<Simulation*> sims);

    // Run simulation
    void play(quint16 id);
    void play(QList<quint16> ids);

    // Pauses simulation asap
    void pause(quint16 id);

    // Pauses simulation at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    void pauseAt(quint16 id, quint64 stepToPause);

    // Stops simulation asap
    void stop(quint16 id);

    // Stops simulation at specific step.
    // If current step is already greater than stepToPause, it'll pause asap.
    void stopAt(quint16 id, quint64 stepToStop);

    // Kill process
    void kill(quint16 id);

    // Kill all processes
    void killAll();

    // as we are changing the number of threads available,
    // we have to walk through all the processes
    void setNumThreads(quint8 threads);

signals:
    void killed(quint16 id);

private:
    int m_threads;
    QList<quint16> m_runningProcesses;
    QList<quint16> m_queuedProcesses;
    QList<quint16> m_processesToKill;
    QMap<quint16, Simulation*> m_processes;

    // method called by a QtConcurrent::run()
    quint16 runThread(quint16 id);

    // watcher
    // trigged when a process ends
    void threadFinished();
};

#endif // PROCESSESMGR_H
