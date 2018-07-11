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

#include <vector>

#include "abstractgraph.h"
#include "abstractplugin.h"

namespace evoplex {

class AbstractModelInterface
{
public:
    // destructor
    virtual ~AbstractModelInterface() = default;

    // Implements the metaheuristic.
    // That is, it contains all the logic to perform ONE step.
    // Return true if algorithm is good for another step or false to stop asap.
    virtual bool algorithmStep() = 0;

    // This method allows you to custom outputs which, for example,
    // might be used by the GUI to generate custom plots or to be stored in a file.
    // The requested "header" must be defined in the modelMetaData.json file.
    virtual std::vector<Value> customOutputs(const Values& inputs) const = 0;
};

class AbstractModel : public AbstractModelInterface, public AbstractPlugin
{
    friend class Trial;

public:
    AbstractGraph* graph() const;
    int currStep() const;
    int status() const;

    inline const Nodes& nodes() const;
    inline const NodePtr& node(const int nodeId) const;
    inline const Edges& edges() const;
    inline const EdgePtr& edge(const int edgeId) const;
    inline const EdgePtr& edge(const int originId, const int neighbourId) const;

    Values customOutputs(const Values& inputs) const override;
};

/************************************************************************
   AbstractModel: Inline member functions
 ************************************************************************/

inline const Nodes& AbstractModel::nodes() const
{ return graph()->nodes(); }

inline const NodePtr& AbstractModel::node(const int nodeId) const
{ return graph()->node(nodeId); }

inline const Edges& AbstractModel::edges() const
{ return graph()->edges(); }

inline const EdgePtr& AbstractModel::edge(const int edgeId) const
{ return graph()->edges().at(edgeId); }

inline const EdgePtr& AbstractModel::edge(const int originId, const int neighbourId) const
{ return node(originId)->outEdges().at(neighbourId); }


} // evoplex
#endif // ABSTRACT_MODEL_H
