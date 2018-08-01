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

#ifndef ABSTRACT_MODEL_H
#define ABSTRACT_MODEL_H

#include <memory.h>
#include <vector>

#include "abstractplugin.h"
#include "abstractgraph.h"
#include "edges.h"
#include "nodes.h"

namespace evoplex {

class AbstractModelInterface
{
public:
    // destructor
    virtual ~AbstractModelInterface() = default;

    // It is executed before the algorithmStep() loop
    // The default implementation of this method does nothing.
    virtual void beforeLoop() = 0;

    // It is executed in a loop and must contain all the logic to perform ONE step.
    // Return true if algorithm is good for another step or false to stop asap.
    virtual bool algorithmStep() = 0;

    // It is executed after the algorithmStep() loop ends.
    // The default implementation of this method does nothing.
    virtual void afterLoop() = 0;

    // It allows implementing custom outputs which can be plotted or stored
    // in a file through Evoplex. The "inputs" must be defined in the
    // metaData.json file. If an experiment requests some custom output,
    // this method will be called once at each time step, receiving the
    // requested inputs.
    virtual Values customOutputs(const Values& inputs) const = 0;
};

class AbstractModel : public AbstractPlugin, public AbstractModelInterface
{
    friend class Trial;

public:
    AbstractGraph* graph() const;

    // current time step
    int step() const;

    // last time step
    int lastStep() const;

    inline const Nodes& nodes() const;
    inline Node node(int nodeId) const;
    inline const Edges& edges() const;
    inline const Edge& edge(int edgeId) const;
    inline const Edge& edge(int originId, int neighbourId) const;

    inline bool init() override;
    inline void beforeLoop() override {}
    inline bool algorithmStep() override;
    inline void afterLoop() override {}
    Values customOutputs(const Values& inputs) const override;

protected:
    AbstractModel() = default;
    ~AbstractModel() override = default;
};

/************************************************************************
   AbstractModel: Inline member functions
 ************************************************************************/

inline const Nodes& AbstractModel::nodes() const
{ return graph()->nodes(); }

inline Node AbstractModel::node(int nodeId) const
{ return graph()->node(nodeId); }

inline const Edges& AbstractModel::edges() const
{ return graph()->edges(); }

inline const Edge& AbstractModel::edge(int edgeId) const
{ return graph()->edge(edgeId); }

inline const Edge &AbstractModel::edge(int originId, int neighbourId) const
{ return node(originId).outEdges().at(neighbourId); }

inline bool AbstractModel::init()
{ return false; }

inline bool AbstractModel::algorithmStep()
{ return false; }

} // evoplex
#endif // ABSTRACT_MODEL_H
