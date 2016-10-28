/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef HOPD_H
#define HOPD_H

#include <QList>
#include <QMetaProperty>
#include <QObject>

#include "../src/core/graph.h"
#include "../src/core/interfaces.h"

class Model: public IModel
{
    Q_OBJECT
    Q_INTERFACES(IModel)

    // we have to define all parameters used by the model
    // anything that you define here will be displayed.
    Q_PROPERTY(double b MEMBER m_b)
    Q_PROPERTY(double l MEMBER m_l)

public:
    Model();
    ~Model();

    void init(Graph* graph, QVariantMap params);

    void algorithmStep();

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
