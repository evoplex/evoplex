/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTMGR_H
#define EXPERIMENTMGR_H

#include <QObject>
#include <QTimer>
#include <QVector>

class Experiment;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

public:
    explicit ExperimentsMgr(int threads);
    ~ExperimentsMgr();

    void run(Experiment* exp);

    // Kill trial
    void kill(Experiment* exp);

    // Kill all trials
    void killAll();

    // as we are changing the number of threads available,
    // we have to walk through all the processes
    void setNumThreads(int threads);

signals:
    void statusChanged(Experiment* exp);
    void progressUpdated(Experiment* exp);
    void killed(int id);

private slots:
    void updateProgressValues();

private:
    int m_threads;
    QTimer* m_timer; // timer to update the progress value of all running experiments
    QVector<Experiment*> m_running;
    QVector<Experiment*> m_queued;
    QVector<Experiment*> m_idle;
    QVector<Experiment*> m_toKill;

    // trigged when an experiment ends (futurewatcher)
    void finished(Experiment* exp);
};

#endif // EXPERIMENTMGR_H
