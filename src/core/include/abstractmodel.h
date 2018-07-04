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

#include "abstractgraph.h"
#include "baseplugin.h"

namespace evoplex {

class BaseModel : public BasePlugin
{
    friend class Experiment;

public:
    inline AbstractGraph* graph() const;
    inline const Nodes& nodes() const;
    inline const NodePtr& node(const int nodeId) const;
    inline const Edges& edges() const;
    inline const EdgePtr& edge(const int edgeId) const;
    inline const EdgePtr& edge(const int originId, const int neighbourId) const;
    inline int currStep() const;
    inline int status() const;

protected:
    explicit BaseModel() : BasePlugin(), m_graph(nullptr), m_currStep(0), m_status(0) {}
    virtual ~BaseModel() {
        delete m_graph;
        delete m_prg;
    }

private:
    AbstractGraph* m_graph;
    int m_currStep;
    int m_status;

    // takes the ownership of the graph and the PRG
    inline bool setup(PRG* prg, const Attributes* attrs, AbstractGraph* graphObj);
};

class AbstractModel : public BaseModel
{
public:
    // destructor
    virtual ~AbstractModel() {}

    // This method is called before the actual simulation and
    // is mainly used to set the environment and parameters.
    // Return false if anything goes wrong with the initialization.
    virtual bool init() = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    // If return is true, algorithm converged and simulation will stop asap.
    virtual bool algorithmStep() = 0;

    // This method allows you to custom outputs which, for example,
    // might be used by the GUI to generate custom plots or to be stored in a file.
    // The requested "header" must be defined in the modelMetaData.json file.
    virtual std::vector<Value> customOutputs(const Values& inputs) const {
        Q_UNUSED(inputs);
        return std::vector<Value>();
    }
};

/************************************************************************
   BaseModel: Inline member functions
 ************************************************************************/

inline AbstractGraph* BaseModel::graph() const
{ return m_graph; }

inline const Nodes& BaseModel::nodes() const
{ return m_graph->nodes(); }

inline const NodePtr& BaseModel::node(const int nodeId) const
{ return m_graph->nodes().at(nodeId); }

inline const Edges& BaseModel::edges() const
{ return m_graph->edges(); }

inline const EdgePtr& BaseModel::edge(const int edgeId) const
{ return m_graph->edges().at(edgeId); }

inline const EdgePtr& BaseModel::edge(const int originId, const int neighbourId) const
{ return node(originId)->outEdges().at(neighbourId); }

inline int BaseModel::currStep() const
{ return m_currStep; }

inline int BaseModel::status() const
{ return m_status; }

inline bool BaseModel::setup(PRG* prg, const Attributes* attrs, AbstractGraph* graphObj) {
    if (BasePlugin::setup(prg, attrs)) {
        m_graph = graphObj;
        m_currStep = 0;
    }
    return m_graph != nullptr;
}


} // evoplex
#endif // ABSTRACT_MODEL_H
