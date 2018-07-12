/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef TRIAL_H
#define TRIAL_H

#include <unordered_map>
#include <QRunnable>

#include "enum.h"

namespace evoplex {

class AbstractGraph;
class AbstractModel;
class Experiment;
class PRG;
class Trial;

using Trials = std::unordered_map<quint16, Trial*>;

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
public:
    explicit Trial(const quint16 id, Experiment* exp);
    ~Trial() override;

    // Here is where the simulation is performed.
    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void run() override;

    GraphType graphType() const;

    inline quint16 id() const;
    inline int step() const;
    inline Status status() const;

    inline PRG* prg() const;
    inline const AbstractModel* model() const;
    inline AbstractGraph* graph() const;

private:
    const quint16 m_id;
    Experiment* m_exp;
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
    bool writeCachedSteps();

    bool _run();
};

/************************************************************************
   Trial: Inline member functions
 ************************************************************************/

inline quint16 Trial::id() const
{ return m_id; }

inline int Trial::step() const
{ return m_step; }

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
