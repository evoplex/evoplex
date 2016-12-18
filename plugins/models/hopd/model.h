/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef HOPD_H
#define HOPD_H

#include <QObject>
#include <QVariantHash>

#include "../src/core/abstractgraph.h"
#include "../src/core/imodel.h"

class Model: public IModel
{
    Q_OBJECT
    Q_INTERFACES(IModel)

public:
    Model();
    ~Model();

    bool init(AbstractGraph* graph, const QVariantHash& modelParams);

    bool algorithmStep();

    QVariantHash paramsSpace() const;
    AbstractAgent *newDefaultAgent() const;
    QVariantHash getModelParams() const;
    QVariantHash getInspectorParams() const;

private:
    AbstractGraph* m_graph;

    double m_b;
    double m_l;
};

// This class will be loaded as a plugin.
class PMModel: public QObject, public IPluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginModel")
    Q_INTERFACES(IPluginModel)

public:
    IModel* create() { return qobject_cast<IModel*>(new Model());  }
    const QString& name() { return "hopd"; }
    const QString& description() { return "optional prisoner's dilemma"; }
    const QString& author() { return "author"; }
};

#endif // HOPD_H
