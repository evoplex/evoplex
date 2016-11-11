/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef HOPD_H
#define HOPD_H

#include <QObject>
#include <QVariantHash>

#include "../src/core/graph.h"
#include "../src/core/interfaces.h"

class Model: public IModel
{
    Q_OBJECT
    Q_INTERFACES(IModel)

public:
    Model();
    ~Model();

    bool init(Graph* graph, const QVariantHash& modelParams);

    bool algorithmStep();

    QVariantHash agentParamsDomain();
    IAgent* newDefaultAgent();

    QVariantHash getModelParams();
    QVariantHash getInspectorParams();

private:
    Graph* m_graph;

    double m_b;
    double m_l;
};

// This class will be loaded as a plugin.
class ModelFactory: public QObject, public IModelFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.evoplex.IModelFactory")
    Q_INTERFACES(IModelFactory)

public:
    IModel* create() { return qobject_cast<IModel*>(new Model());  }
    QString name() { return "hopd"; }
    QString description() { return "optional prisoner's dilemma"; }
    QString author() { return "author"; }
};

#endif // HOPD_H
