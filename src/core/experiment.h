/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QHash>
#include <QMutex>
#include <QString>
#include <QTextStream>

#include "experimentsmgr.h"
#include "mainapp.h"
#include "constants.h"
#include "output.h"

namespace evoplex {

// Evoplex assumes that any experiment belong to a project.
// Cosidering that a project might have a massive amount of experiments, this class
// should be as light as possible and should not do any parameter validation. So,
// we assume that everything is valid at this point.
class Experiment
{
    friend class ExperimentsMgr;

public:
    enum Status {
        INVALID,  // something went wrong
        READY,    // ready for another step
        QUEUED,   // queued to run
        RUNNING,  // running in a work thread
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

    explicit Experiment(MainApp* mainApp, int id, int projId, Attributes* generalAttrs,
            Attributes* modelAttrs, Attributes* graphAttrs, std::vector<Output*> fileOutputs);

    ~Experiment();

    void init(Attributes* generalAttrs, Attributes* modelAttrs,
              Attributes* graphAttrs, std::vector<Output*> fileOutputs);

    void reset();

    // Updates the progress value.
    // This method might be expensive!
    void updateProgressValue();
    inline quint16 getProgress() const { return m_progress; }

    // Here is where the actual simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processTrial(const int& trialId);

    void toggle();

    // run all trials
    inline void play() { m_mainApp->getExperimentsMgr()->play(this); }
    void playNext();

    // pause all trials at a specific step
    inline int pauseAt() const { return m_pauseAt; }
    inline void setPauseAt(int step) { m_pauseAt = step > m_stopAt ? m_stopAt : step; }

    // stop all trials at a specific step
    inline int stopAt() const { return m_stopAt; }
    inline void setStopAt(int step) { m_stopAt = step > EVOPLEX_MAX_STEPS ? EVOPLEX_MAX_STEPS : step; }

    // pause all trials asap
    inline void pause() { m_pauseAt = 0; }

    // stop all trials asap
    inline void stop() { pause(); m_stopAt = 0; play(); }

    inline const Status expStatus() const { return m_expStatus; }
    inline void setExpStatus(Status s) { m_mutex.lock(); m_expStatus = s; m_mutex.unlock(); }

    inline bool autoDelete() const { return m_autoDelete; }
    inline void setAutoDelete(bool b) { m_autoDelete = b; }

    inline void addOutput(Output* output) { m_extraOutputs.emplace_back(output); }
    Output* searchOutput(const Output* find);

    AbstractGraph* graph(int trialId) const;
    inline int id() const { return m_id; }
    inline int projId() const { return m_projId; }
    inline int numTrials() const { return m_numTrials; }
    inline const QHash<int, Trial>& trials() const { return m_trials; }
    inline const Attributes* generalAttrs() const { return m_generalAttrs; }
    inline const Attributes* modelAttrs() const { return m_modelAttrs; }
    inline const Attributes* graphAttrs() const { return m_graphAttrs; }
    inline const QString& modelId() const { return m_modelPlugin->uid; }
    inline const QString& graphId() const { return m_graphPlugin->uid; }
    inline const MainApp::ModelPlugin* modelPlugin() const { return m_modelPlugin; }

private:
    QMutex m_mutex;
    MainApp* m_mainApp;
    const int m_id;
    const int m_projId;

    Attributes* m_generalAttrs;
    Attributes* m_modelAttrs;
    Attributes* m_graphAttrs;
    const MainApp::GraphPlugin* m_graphPlugin;
    const MainApp::ModelPlugin* m_modelPlugin;
    int m_numTrials;
    int m_seed;
    bool m_autoDelete;
    int m_stopAt;

    QString m_fileHeader;   // file header is the same for all trials; let's save it then
    std::vector<Output*> m_fileOutputs;
    std::vector<Output*> m_extraOutputs;
    QHash<int, QTextStream*> m_fileStreams; // <trialId, stream>

    int m_pauseAt;
    Status m_expStatus;
    quint16 m_progress; // current progress value [0, 360]

    QHash<int, Trial> m_trials;

    // It holds the initial population for further use (internal only).
    // If this experiment has only one trial, then it won't be used anyway.
    // If the experiment has 2 or more trials, it'll be used in the last trial.
    Agents m_clonableAgents;

    // We can safely consider that all parameters are valid at this point.
    // However, some things might fail (eg, missing agents, broken graph etc),
    // and, in that case, an invalid Trial() is returned.
    Trial createTrial(const int trialId);

    // The trials are meant to have the same initial population.
    // So, considering that it might be a very expensive operation (eg, I/O),
    // this method will try to do the heavy stuff only once, storing the
    // initial population in the 'm_clonableAgents' vector. Except when
    // the experiment has only one trial.
    // This method is NOT thread-safe.
    Agents createAgents();

    // clone a population of agents
    Agents cloneAgents(const Agents& agents) const;

    void deleteTrials();

    void writeStep(const int trialId);
};
}

// Lets make the Experiment pointer known to QMetaType
// It enable us to convert an Experiment* to a QVariant for example
Q_DECLARE_METATYPE(evoplex::Experiment*)

#endif // EXPERIMENT_H
