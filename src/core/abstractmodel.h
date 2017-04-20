/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_MODEL_H
#define ABSTRACT_MODEL_H

#include <QObject>
#include <QVariantHash>
#include <QtPlugin>

#include "core/abstractagent.h"
#include "core/abstractgraph.h"
#include "utils/prg.h"

class AbstractBaseModel
{
public:
    // destructor
    virtual ~AbstractBaseModel() {
        delete m_graph;
        m_graph = nullptr;
        delete m_prg;
        m_prg = nullptr;
        qDeleteAll(m_graph->getPopulation());
    }

protected:
    inline AbstractGraph* graph() { return m_graph; }
    inline PRG* prg() { return m_prg; }

private:
    friend class Experiment;

    AbstractGraph* m_graph;
    PRG* m_prg;

    inline bool setup(int seed, AbstractGraph* graphObj) {
        m_prg = new PRG(seed);
        m_graph = graphObj;
    }
};

class AbstractModel : public AbstractBaseModel
{
public:
    // destructor
    virtual ~AbstractModel() {}

    // This method is called before the actual simulation and
    // is mainly used to set the environment and parameters.
    // Return false if anything goes wrong with the initialization.
    virtual bool init(const QVariantHash& modelParams) = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    // If return is true, algorithm converged and simulation will stop asap.
    virtual bool algorithmStep() = 0;

    // This method allows you to create custom outputs which, for example,
    // might be used by the GUI to generate custom plots or to be stored in a file.
    virtual QVariantList requestCustomOutputs(QStringList requestedHeader) const {}
};

class IPluginModel
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginModel() {}

    // create the real model object.
    virtual AbstractModel* create() = 0;
};
Q_DECLARE_INTERFACE(IPluginModel, "org.evoplex.IPluginModel")


#define REGISTER_MODEL(CLASSNAME)                                           \
    class PM_##CLASSNAME: public QObject, public IPluginModel               \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginModel"                        \
                      FILE "modelMetaData.json")                            \
                                                                            \
    Q_INTERFACES(IPluginModel)                                              \
    public:                                                                 \
        AbstractModel* create() {                                           \
            return dynamic_cast<AbstractModel*>(new CLASSNAME());           \
        }                                                                   \
    };

#endif // ABSTRACT_MODEL_H
