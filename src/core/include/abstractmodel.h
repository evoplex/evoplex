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
    const QString& graphId() const;
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

    // AbstractModelInterface stuff
    // the default implementation of the methods below do nothing
    inline void beforeLoop() override {}
    inline void afterLoop() override {}
    inline Values customOutputs(const Values& inputs) const override
    { Q_UNUSED(inputs); return Values(); }

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

} // evoplex
#endif // ABSTRACT_MODEL_H
