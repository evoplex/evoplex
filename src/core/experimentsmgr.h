/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTMGR_H
#define EXPERIMENTMGR_H

#include <QObject>
#include <QTimer>
#include <QVector>

namespace evoplex {

class Experiment;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

public:
    explicit ExperimentsMgr(int threads);
    ~ExperimentsMgr();

    void play(Experiment* exp);

    void kill(Experiment* exp);

    void killAll();

    inline const int maxThreadsCount() const { return m_threads; }
    void setMaxThreadCount(const int newValue);

signals:
    void statusChanged(Experiment* exp);
    void progressUpdated(Experiment* exp);
    void killed(int id);

public slots:
    void clearQueue();
    void removeFromQueue(Experiment* exp);

private slots:
    void updateProgressValues();

private:
    int m_threads;
    QTimer* m_timer; // timer to update the progress value of all running experiments
    std::list<Experiment*> m_running;
    std::list<Experiment*> m_queued;
    std::list<Experiment*> m_idle;
    QVector<Experiment*> m_toKill;

    // trigged when an experiment ends (futurewatcher)
    void finished(Experiment* exp);
};
}

#endif // EXPERIMENTMGR_H
