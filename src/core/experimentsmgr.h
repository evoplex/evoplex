/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTMGR_H
#define EXPERIMENTMGR_H

#include <QObject>
#include <QTimer>
#include <list>

namespace evoplex {

class Experiment;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

public:
    explicit ExperimentsMgr();
    ~ExperimentsMgr();

    void play(Experiment* exp);

    inline const int maxThreadsCount() const { return m_threads; }
    void setMaxThreadCount(const int newValue);

signals:
    void statusChanged(Experiment* exp);
    void progressUpdated(Experiment* exp);
    void restarted(Experiment* exp);
    void trialCreated(Experiment* exp, int trialId);
    void trialsDeleted(Experiment* exp);

public slots:
    void clearQueue();
    void clearIdle();
    void removeFromQueue(Experiment* exp);

private slots:
    void updateProgressValues();

private:
    int m_threads;
    QTimer* m_timer; // timer to update the progress value of all running experiments
    std::list<Experiment*> m_running;
    std::list<Experiment*> m_queued;
    std::list<Experiment*> m_idle;

    // trigged when an experiment ends (futurewatcher)
    void finished(Experiment* exp);
};
}

#endif // EXPERIMENTMGR_H
