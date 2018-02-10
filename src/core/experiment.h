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
#include <unordered_map>
#include <vector>

#include "experimentsmgr.h"
#include "mainapp.h"
#include "constants.h"
#include "output.h"
#include "graphplugin.h"
#include "modelplugin.h"

namespace evoplex
{
// Evoplex assumes that any experiment belong to a project.
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

    struct ExperimentInputs {
        const Attributes* generalAttrs;
        const Attributes* modelAttrs;
        const Attributes* graphAttrs;
        const std::vector<Output*> fileOutputs;

        ExperimentInputs(const Attributes* general, const Attributes* model,
                         const Attributes* graph, const std::vector<Output*> outputs)
            : generalAttrs(general), modelAttrs(model), graphAttrs(graph), fileOutputs(outputs) {}

        ~ExperimentInputs() {
            delete generalAttrs;
            delete modelAttrs;
            delete graphAttrs;
            qDeleteAll(fileOutputs);
        }
    };

    // Read and validates the experiment inputs.
    // We assume that all graph/model attributes start with 'uid_'. It is very
    // important to avoid clashes between different attributes which use the same name.
    // @return nullptr if unsuccessful
    static ExperimentInputs* readInputs(const MainApp* mainApp,
            const QStringList& header, const QStringList& values, QString& errorMsg);

    explicit Experiment(MainApp* mainApp, int id, ExperimentInputs* inputs, Project* project);

    ~Experiment();

    bool init(ExperimentInputs* inputs);

    void reset();

    // Updates the progress value.
    // This method might be expensive!
    void updateProgressValue();
    inline quint16 progress() const { return m_progress; }

    // Here is where the actual simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processTrial(const int& trialId);

    void toggle();

    // run all trials
    inline void play() { m_mainApp->expMgr()->play(this); }
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

    inline quint16 delay() const { return m_delay; }
    inline void setDelay(quint16 delay) { m_delay = delay; }

    inline const Status expStatus() const { return m_expStatus; }
    inline void setExpStatus(Status s) { m_mutex.lock(); m_expStatus = s; m_mutex.unlock(); }

    inline bool autoDelete() const { return m_autoDelete; }
    inline void setAutoDelete(bool b) { m_autoDelete = b; }

    inline bool hasOutputs() const { return !m_inputs->fileOutputs.empty() || !m_extraOutputs.empty(); }
    inline void addOutput(Output* output) { m_extraOutputs.emplace_back(output); }
    bool removeOutput(Output* output);
    Output* searchOutput(const Output* find);

    AbstractModel* trial(int trialId) const;
    inline const std::unordered_map<int, AbstractModel*>& trials() const { return m_trials; }

    inline int id() const { return m_id; }
    inline Project* project() const { return m_project; }
    inline int numTrials() const { return m_numTrials; }
    inline const ExperimentInputs* inputs() const { return m_inputs; }
    inline const QString& modelId() const { return m_modelPlugin->id(); }
    inline const QString& graphId() const { return m_graphPlugin->id(); }
    inline const ModelPlugin* modelPlugin() const { return m_modelPlugin; }

private:
    QMutex m_mutex;
    MainApp* m_mainApp;
    const int m_id;
    Project* m_project;

    const ExperimentInputs* m_inputs;
    const GraphPlugin* m_graphPlugin;
    const ModelPlugin* m_modelPlugin;
    int m_numTrials;
    bool m_autoDelete;
    int m_stopAt;

    QString m_fileHeader;   // file header is the same for all trials; let's save it then
    std::vector<Output*> m_extraOutputs;
    std::unordered_map<int, QTextStream*> m_fileStreams; // <trialId, stream>

    int m_pauseAt;
    Status m_expStatus;
    quint16 m_progress; // current progress value [0, 360]
    quint16 m_delay;

    // A trial is part of an experiment which might have several other trials.
    // All trials of an experiment are meant to use exactly the same parameters
    // and population of agents. Be aware that each trial will use the root
    // seed incremented by 1. For exemple, if an experiment with 3 trials has
    // the seed '111', the seeds of the trials will be '111', '112' and '113'.
    std::unordered_map<int, AbstractModel*> m_trials;

    // It holds the initial population for further use (internal only).
    // If this experiment has only one trial, then it won't be used anyway.
    // If the experiment has 2 or more trials, it'll be used in the last trial.
    Agents m_clonableAgents;

    // We can safely consider that all parameters are valid at this point.
    // However, some things might fail (eg, missing agents, broken graph etc),
    // and, in that case, a null pointer is returned.
    AbstractModel* createTrial(const int trialId);

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

    void writeCachedSteps(const int trialId);
};
}

// Lets make the Experiment pointer known to QMetaType
// It enable us to convert an Experiment* to a QVariant for example
Q_DECLARE_METATYPE(evoplex::Experiment*)

#endif // EXPERIMENT_H
