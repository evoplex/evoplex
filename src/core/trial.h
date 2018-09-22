/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRIAL_H
#define TRIAL_H

#include <unordered_map>
#include <QRunnable>

#include "enum.h"
#include "experiment.h"

namespace evoplex {

/**
 * A trial is part of an experiment which might have several other trials.
 * All trials of an experiment have exactly the same initial conditions,
 * i.e., all attributes and set of nodes are the same.
 *
 * The only difference is that each trial uses a different PRG seed, which
 * is incremented by 1 from the root seed. For exemple, if an experiment
 * seeded with '111' has 3 trials, the seeds of the trials will be '111',
 * '112' and '113'.
 */
class Trial : public QRunnable
{
    friend class ExperimentsMgr;

public:
    explicit Trial(const quint16 id, ExperimentPtr exp);
    ~Trial() override;

    // Here is where the simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void run() override;

    const QString& graphId() const;
    GraphType graphType() const;

    inline quint16 id() const;
    inline Status status() const;
    inline int step() const;
    inline int stopAt() const;

    inline PRG* prg() const;
    inline const AbstractModel* model() const;
    inline AbstractGraph* graph() const;

private:
    const quint16 m_id;
    ExperimentPtr m_exp;
    int m_step;
    Status m_status;

    PRG* m_prg;
    AbstractGraph* m_graph;
    AbstractModel* m_model;

    // We can safely consider that all parameters are valid at this point.
    // However, some things might fail (eg, missing nodes, broken graph etc),
    // and, in that case, false is returned.
    bool init();

    // The main loop for calling the model steps
    // Returns true if it has a next step
    bool runSteps();

    // If any file output is set, it'll write the cached steps to file.
    bool writeCachedSteps(const Experiment* exp) const;
};

/************************************************************************
   Trial: Inline member functions
 ************************************************************************/

inline quint16 Trial::id() const
{ return m_id; }

inline int Trial::step() const
{ return m_step; }

inline int Trial::stopAt() const
{ return m_exp->stopAt(); }

inline Status Trial::status() const
{ return m_status; }

inline PRG* Trial::prg() const
{ return m_prg; }

inline const AbstractModel* Trial::model() const
{ return m_model; }

inline AbstractGraph* Trial::graph() const
{ return m_graph; }

} // evoplex
#endif // TRIAL_H
