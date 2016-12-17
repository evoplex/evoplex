/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef IMODEL_H
#define IMODEL_H

#include <QObject>
#include <QVariantHash>
#include <QtPlugin>

#include "core/abstractagent.h"
#include "core/abstractgraph.h"

class IModel: public QObject
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IModel() {}

    // this method is called before the actual simulation and
    // is mainly used to set the environment and parameters
    virtual bool init(AbstractGraph* graph, const QVariantHash& modelParams) = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    // If return is true, algorithm converged and simulation will stop asap.
    virtual bool algorithmStep() = 0;

    // return a QVariantHash with all parameters used to define an agent.
    // use {} to define a set of possible things. eg, {0,1} zero and one
    // use [] to define a continuous interval. eg., [0,1] zero to one (including)
    virtual QVariantHash paramsSpace() const = 0;

    // return a new IAgent with default parameters
    virtual AbstractAgent* newDefaultAgent() const = 0;

    // return the current status of all parameters
    // it might be a bit slow -- be careful!
    virtual QVariantHash getModelParams() const = 0;

    // return the current status of all things that can be inspected over time.
    // it might be a bit slow -- be careful!
    virtual QVariantHash getInspectorParams() const = 0;
};

class IPluginModel
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginModel() {}

    // create the real model object.
    virtual IModel* create() = 0;

    // author's name
    virtual QString author() = 0;

    // model's name
    virtual QString name() = 0;

    // model's description
    virtual QString description() = 0;
};

Q_DECLARE_INTERFACE(IModel, "org.evoplex.IModel")
Q_DECLARE_INTERFACE(IPluginModel, "org.evoplex.IPluginModel")

#endif // IMODEL_H
