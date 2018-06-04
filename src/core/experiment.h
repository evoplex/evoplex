/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QHash>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "experimentsmgr.h"
#include "mainapp.h"
#include "constants.h"
#include "output.h"
#include "graphplugin.h"
#include "modelplugin.h"
#include "utils.h"

namespace evoplex
{
// Evoplex assumes that any experiment belong to a project.
class Experiment : public QObject
{
    Q_OBJECT

    friend class ExperimentsMgr;
    friend class TrialRunnable;

public:
    enum Status {
        INVALID,  // something went wrong
        READY,    // ready for another step
        QUEUED,   // queued to run
        RUNNING,  // running in a work thread
        FINISHED, // all is done
    };
    Q_ENUM(Status)

    struct ExperimentInputs {
        const Attributes* generalAttrs;
        const Attributes* modelAttrs;
        const Attributes* graphAttrs;
        const std::vector<Cache*> fileCaches;

        ExperimentInputs(const Attributes* general, const Attributes* model,
                         const Attributes* graph, const std::vector<Cache*> caches)
            : generalAttrs(general), modelAttrs(model), graphAttrs(graph), fileCaches(caches) {}

        ~ExperimentInputs() {
            delete generalAttrs;
            delete modelAttrs;
            delete graphAttrs;
            for (Cache* c : fileCaches)
                c->deleteCache();
        }
    };

    // Read and validates the experiment inputs.
    // We assume that all graph/model attributes start with 'uid_'. It is very
    // important to avoid clashes between different attributes which use the same name.
    // @return nullptr if unsuccessful
    static ExperimentInputs* readInputs(const MainApp* mainApp,
            const QStringList& header, const QStringList& values, QString& errorMsg);

    explicit Experiment(MainApp* mainApp, ExperimentInputs* inputs, ProjectSP project);

    ~Experiment();

    bool init(ExperimentInputs* inputs, QString& error);

    void reset();

    inline quint16 progress() const { return m_progress; }

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
    void setExpStatus(Status s);

    inline bool autoDeleteTrials() const { return m_autoDeleteTrials; }
    inline void setAutoDeleteTrials(bool b) { m_autoDeleteTrials = b; }

    inline bool hasOutputs() const { return !m_outputs.empty(); }
    inline void addOutput(OutputSP output) { m_outputs.insert(output); }
    bool removeOutput(OutputSP output);
    OutputSP searchOutput(const OutputSP find);

    AbstractModel* trial(int trialId) const;
    inline const std::unordered_map<int, AbstractModel*>& trials() const { return m_trials; }

    inline int id() const { return m_id; }
    inline ProjectSP project() const { return m_project; }
    inline int numTrials() const { return m_numTrials; }
    inline const ExperimentInputs* inputs() const { return m_inputs; }
    inline const QString& modelId() const { return m_modelPlugin->id(); }
    inline const QString& graphId() const { return m_graphPlugin->id(); }
    inline const ModelPlugin* modelPlugin() const { return m_modelPlugin; }

signals:
    void trialCreated(int trialId);
    void restarted();
    void progressUpdated();
    void statusChanged(Experiment::Status);

private slots:
    // Updates the progress value.
    // This method might be expensive!
    void updateProgressValue();

private:
    QMutex m_mutex;
    MainApp* m_mainApp;
    const int m_id;
    ProjectSP m_project;

    const ExperimentInputs* m_inputs;
    const GraphPlugin* m_graphPlugin;
    const ModelPlugin* m_modelPlugin;
    int m_numTrials;
    bool m_autoDeleteTrials;
    int m_stopAt;

    QString m_fileHeader;   // file header is the same for all trials; let's save it then
    QString m_filePathPrefix;
    std::unordered_set<OutputSP> m_outputs;

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

    // Here is where the actual simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processTrial(const int& trialId);

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

    bool writeCachedSteps(const int trialId);
};
}

// Lets make the Experiment pointer known to QMetaType
// It enable us to convert an Experiment* to a QVariant for example
Q_DECLARE_METATYPE(evoplex::Experiment*)
Q_DECLARE_METATYPE(const evoplex::Experiment*)

#endif // EXPERIMENT_H
