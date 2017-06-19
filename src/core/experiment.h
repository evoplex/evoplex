/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QMutex>
#include <QString>
#include <QVariantHash>
#include <QVector>

#include "core/mainapp.h"
#include "utils/constants.h"

// Evoplex assumes that any experiment belong to a project.
// Cosidering that a project might have a massive amount of experiments, this class
// should be as light as possible and should not do any parameter validation. So,
// we assume that everything is valid at this point.
class Experiment
{
public:
    enum Status {
        INVALID,  // something went wrong
        READY,    // ready for another step
        RUNNING,  // trial is running in a work thread
        FINISHED, // all is done
    };

    // A trial is part of an experiment which might have several other trials.
    // All trials of an experiment are meant to use exactly the same parameters
    // and population of agents. Be aware that each trial will use the root
    // seed incremented by 1. For exemple, if an experiment with 3 trials has
    // the seed '111', the seeds of the trials will be '111', '112' and '113'.
    struct Trial {
        AbstractModel* modelObj = nullptr;
        int currentStep = 0;
        Status status = INVALID;
    };

    // constructor
    explicit Experiment(MainApp* mainApp, int id, int projId, const QVariantHash& generalParams,
        const QVariantHash& modelParams, const QVariantHash& graphParams);

    // destructor
    virtual ~Experiment() {}

    // Here is where the actual simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processTrial(const int& trialId);

    // run all trials
    void run();

    void finished();

    // pause all trials at a specific step
    inline void pauseAt(int step) { m_pauseAt = step > m_stopAt ? m_stopAt : step; }
    // stop all trials at a specific step
    inline void stopAt(int step) { m_stopAt = step > EVOPLEX_MAX_STEPS ? EVOPLEX_MAX_STEPS : step; }
    // pause all trials asap
    inline void pause() { m_pauseAt = 0; }
    // stop all trials asap
    inline void stop() { pause(); m_stopAt = 0; run(); }

    // getters
    inline int getId() { return m_id; }
    inline int getProjId() { return m_projId; }
    inline int getPauseAt() { return m_pauseAt; }
    inline int getStopAt() { return m_stopAt; }
    inline int getNumTrials() { return m_numTrials; }
    inline const QVariantHash& getGeneralParams() { return m_generalParams; }
    inline const QVariantHash& getModelParams() { return m_modelParams; }
    inline const QVariantHash& getGraphParams() { return m_graphParams; }

private:
    QMutex m_mutex;
    MainApp* m_mainApp;
    const int m_id;
    const int m_projId;

    const QVariantHash m_generalParams;
    const QVariantHash m_modelParams;
    const QVariantHash m_graphParams;

    const MainApp::GraphPlugin* m_graphPlugin;
    const MainApp::ModelPlugin* m_modelPlugin;
    const int m_numTrials;
    const int m_seed;
    int m_stopAt;
    int m_pauseAt;
    Status m_expStatus;

    QHash<int, Trial> m_trials;

    QVector<AbstractAgent> m_clonableAgents; // holds the initial population for further use

    // We can safely consider that all parameters are valid at this point.
    // However, some things might fail (eg, missing agents, broken graph etc),
    // and, in that case, we return -1 to indicate that something went wrong.
    Trial createTrial(const int& trialSeed);

    // The trials are meant to have the same initial population.
    // So, considering that it might be a very expensive operation (eg, I/O),
    // this method will try to do the heavy stuff only once, storing the
    // initial population in the 'm_clonableAgents' vector. Except when
    // the experiment has only one trial.
    // This method is thread-safe.
    QVector<AbstractAgent> createAgents();

    // clone a population of agents
    QVector<AbstractAgent> cloneAgents(const QVector<AbstractAgent>& agents) const;

    // set experiment status
    inline void setExpStatus(Status s) { m_mutex.lock(); m_expStatus = s; m_mutex.unlock(); }
};

#endif // EXPERIMENT_H
