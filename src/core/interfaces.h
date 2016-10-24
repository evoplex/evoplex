/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QList>
#include <QMetaProperty>
#include <QtPlugin>

#include "graph.h"

class IModel: public QObject
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IModel() {}

    // this method is called before the actuall simulation and
    // is mainly used to set the environment (initial population)
    virtual void init(Graph* graph, QVariantMap params) = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    virtual void algorithmStep() = 0;
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
