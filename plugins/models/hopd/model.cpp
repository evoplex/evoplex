#include <QtDebug>

#include "model.h"

Model::Model()
    : m_graph(NULL)
    , m_b(1.6)
    , m_l(0.3)
{
}

Model::~Model()
{
    delete m_graph;
    m_graph = NULL;
}

bool Model::init(AbstractGraph* graph, const QVariantHash& modelParams)
{
    return true;
}

bool Model::algorithmStep()
{
    qDebug() << "HOPD";
    return false;
}

AbstractAgent* Model::newDefaultAgent() const
{
    AbstractAgent* a = new AbstractAgent();
    a->setProperty("strategy", 0);
    a->setProperty("probToA", 0.0);
    return a;
}

QVariantHash Model::paramsSpace() const
{
    QVariantHash h;
    //
    // agent definition
    //
    // probability to turn to abstainer
    h.insert("probToA", "[0,1]");
    // possible strategies for this model
    // 0 (cooperator), 1 (defector) or 2 (abstainer)
    h.insert("strategy", "{0,1,2}");

    //
    // model stuff
    //
    h.insert("graphType", "{mooreGrid}");
    h.insert("b", "[1,2]");
    h.insert("l", "[0,1]");
    return h;
}

QVariantHash Model::getModelParams() const
{
    QVariantHash h;
    h.insert("b", m_b);   // temptation to defect
    h.insert("l", m_l);   // loner's payoff
    return h;
}

QVariantHash Model::getInspectorParams() const
{
    QVariantHash h;
    // TODO
    return h;
}
