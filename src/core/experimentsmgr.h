/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTMGR_H
#define EXPERIMENTMGR_H

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <QSettings>
#include <list>

namespace evoplex {

class Experiment;

class ExperimentsMgr: public QObject
{
    Q_OBJECT

public:
    explicit ExperimentsMgr();
    ~ExperimentsMgr();

    void resetSettingsToDefault();

    void play(Experiment* exp);

    inline const int maxThreadsCount() const { return m_threads; }
    void setMaxThreadCount(const int newValue);

signals:
    void statusChanged(Experiment* exp);
    void progressUpdated(Experiment* exp);

public slots:
    void clearQueue();
    void clearIdle();
    void removeFromQueue(Experiment* exp);
    void destroy(Experiment* exp);

private slots:
    void updateProgressValues();
    void destroyExperiments();

private:
    QMutex m_mutex;
    QSettings m_userPrefs;
    int m_threads;

    QTimer* m_timerProgress; // update the progress value of all running experiments
    QTimer* m_timerDestroy;

    std::list<Experiment*> m_running;
    std::list<Experiment*> m_queued;
    std::list<Experiment*> m_idle;
    std::list<Experiment*> m_toDestroy;

    // trigged when an experiment ends (futurewatcher)
    // also runs in a work thread
    void finished(Experiment* exp);
};
}

#endif // EXPERIMENTMGR_H
