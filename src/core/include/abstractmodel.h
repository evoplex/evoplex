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

/**
 * @brief Provides a common interface for Model plugins.
 * @see AbstractModel
 * @ingroup AbstractModel
 */
class AbstractModelInterface : public AbstractPlugin
{
public:
    //! Provide a default destructor to keep compilers happy.
    virtual ~AbstractModelInterface() = default;

    using AbstractPlugin::init;

    /**
     * @brief It is executed before the algorithmStep() loop.
     * The default implementation of this function does nothing.
     */
    virtual void beforeLoop() = 0;

    /**
     * @brief It is executed in a loop and must contain all the logic to perform ONE step.
     * @returns true if algorithm is good for another step or false to stop asap.
     */
    virtual bool algorithmStep() = 0;

    /**
     * @brief It is executed after the algorithmStep() loop ends.
     * The default implementation of this function does nothing.
     */
    virtual void afterLoop() = 0;

    /**
     * @brief Allows implementing custom outputs for the model plugin.
     *
     * A "custom output" can be plotted or stored in a file through Evoplex.
     * The \p inputs must be defined in the metadata.json file. If an experiment
     * requests some custom output, this function will be called once at each
     * time step, receiving the requested \p inputs.
     * The default implementation of this function does nothing.
     * @return the Value output for each of the \p inputs
     */
    virtual Values customOutputs(const Values& inputs) const = 0;
};

/**
 * @brief Abstract base class for model plugins.
 * @ingroup AbstractModel
 */
class AbstractModel : public AbstractModelInterface
{
    friend class Trial;

public:
//! @addtogroup ModelAPI
//! @{

    /**
     * @brief Gets the graph id.
     */
    const QString& graphId() const;

    /**
     * @brief Gets the pointer to the current graph.
     */
    AbstractGraph* graph() const;

    /**
     * @brief Gets the current time step.
     */
    int step() const;

    /**
     * @brief Gets the end time step (stopAt).
     */
    int lastStep() const;

    //! @copydoc AbstractGraph::nodes
    inline const Nodes& nodes() const;
    //! @copydoc AbstractGraph::node
    inline Node node(int nodeId) const;

    //! @copydoc AbstractGraph::edges
    inline const Edges& edges() const;
    //! @copydoc AbstractGraph::edge
    inline const Edge& edge(int edgeId) const;
    //! @copydoc AbstractGraph::edge(int originId, int neighbourId) const
    inline const Edge& edge(int originId, int neighbourId) const;

    // AbstractModelInterface stuff
    // the default implementation of the functions below do nothing
    inline void beforeLoop() override {}
    inline void afterLoop() override {}
    inline Values customOutputs(const Values& inputs) const override
    { Q_UNUSED(inputs); return Values(); }

/**@}*/

protected:
    //! constructor
    AbstractModel() = default;
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
{ return graph()->edge(originId, neighbourId); }

} // evoplex
#endif // ABSTRACT_MODEL_H
