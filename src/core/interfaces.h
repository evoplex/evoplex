/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QObject>
#include <QVariantHash>
#include <QtPlugin>

class Graph;

class IAgent
{
public:
    inline const QVariantHash getProperties() {
        return m_properties;
    }

    inline const QVariant getProperty(const QString& name) {
        return m_properties.value(name);
    }

    inline void setProperty(const QString& name, const QVariant& value) {
        m_properties.insert(name, value);
    }

private:
    QVariantHash m_properties;
};

class IModel: public QObject
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IModel() {}

    // this method is called before the actual simulation and
    // is mainly used to set the environment and parameters
    virtual bool init(Graph* graph, const QVariantHash& modelParams) = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    // If return is false, simulation will stop asap.
    virtual bool algorithmStep() = 0;

    // return a QVariantHash with all parameters used to define an agent.
    // use {} to define a set of possible things. eg, {0,1} zero and one
    // use [] to define a continuous interval. eg., [0,1] zero to one (including)
    virtual QVariantHash agentParamsDomain() = 0;

    // return a new IAgent with default parameters
    virtual IAgent* newDefaultAgent() = 0;

    // return the current status of all parameters
    // it might be a bit slow -- be careful!
    virtual QVariantHash getModelParams() = 0;

    // return the current status of all things that can be inspected over time.
    // it might be a bit slow -- be careful!
    virtual QVariantHash getInspectorParams() = 0;
};

class IModelFactory
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IModelFactory() {}

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
Q_DECLARE_INTERFACE(IModelFactory, "org.evoplex.IModelFactory")

#endif // INTERFACES_H
