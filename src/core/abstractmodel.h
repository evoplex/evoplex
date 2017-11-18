/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_MODEL_H
#define ABSTRACT_MODEL_H

#include <QObject>
#include <QtPlugin>

#include "abstractgraph.h"
#include "agent.h"
#include "prg.h"

namespace evoplex {

class AbstractBaseModel
{
    friend class Experiment;

public:
    inline AbstractGraph* graph() const { return m_graph; }
    inline const PRG* prg() const { return m_prg; }
    inline const Attributes* attrs() const { return m_attributes; }
    inline const Value attr(const QString& name) const { return m_attributes->value(name); }
    inline const Value attr(int attrId) const { return m_attributes->value(attrId);  }
    inline const QString& attrName(int attrId) const { return m_attributes->name(attrId); }
    inline const int currStep() const { return m_currStep; }

protected:
    explicit AbstractBaseModel() : m_graph(nullptr), m_prg(nullptr) {}

    virtual ~AbstractBaseModel() {
        delete m_graph;
        m_graph = nullptr;
        delete m_prg;
        m_prg = nullptr;
    }

private:
    AbstractGraph* m_graph;
    PRG* m_prg;
    Attributes* m_attributes;
    int m_currStep;

    // takes the ownership of the graph and the PRG
    inline void setup(PRG* prg, AbstractGraph* graphObj, Attributes* attrs) {
        Q_ASSERT(!m_prg && !m_graph); // make sure it'll be called only once
        m_prg = prg;
        m_graph = graphObj;
        m_attributes = attrs;
        m_currStep = 0;
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
    virtual bool init() = 0;

    // Implements the metaheuristic.
    // That is, it has to contain all the logic to perform ONE step.
    // If return is true, algorithm converged and simulation will stop asap.
    virtual bool algorithmStep() = 0;

    // This method allows you to custom outputs which, for example,
    // might be used by the GUI to generate custom plots or to be stored in a file.
    // The requested "header" must be defined in the modelMetaData.json file.
    virtual std::vector<Value> customOutputs(const Values& inputs) const {
        return std::vector<Value>();
    }
};

class IPluginModel
{
public:
    // provide the destructor to keep compilers happy.
    virtual ~IPluginModel() {}

    // create the real model object.
    virtual AbstractModel* create() = 0;
};
}
Q_DECLARE_INTERFACE(evoplex::IPluginModel, "org.evoplex.IPluginModel")


#define REGISTER_MODEL(CLASSNAME)                                           \
    namespace evoplex {                                                     \
    class PM_##CLASSNAME: public QObject, public IPluginModel               \
    {                                                                       \
    Q_OBJECT                                                                \
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginModel"                        \
                      FILE "modelMetaData.json")                            \
    Q_INTERFACES(evoplex::IPluginModel)                                     \
    public:                                                                 \
        AbstractModel* create() {                                           \
            return dynamic_cast<AbstractModel*>(new CLASSNAME());           \
        }                                                                   \
    };}

#endif // ABSTRACT_MODEL_H
