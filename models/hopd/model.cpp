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

void Model::init(Graph* graph, const QVariantHash& modelParams)
{
}

void Model::algorithmStep()
{
    qDebug() << "HOPD";
}

IAgent* Model::newDefaultAgent()
{
    IAgent* a = new IAgent();
    a->setProperty("strategy", 0);
    a->setProperty("probToA", 0.0);
    return a;
}

QVariantHash Model::agentParamsDomain()
{
    QVariantHash map;
    // possible strategies for this model
    // 0 (cooperator), 1 (defector) or 2 (abstainer)
    map.insert("strategy", "{0,1,2}");
    // probability to turn to abstainer
    map.insert("probToA", "[0,1]");
    return map;
}

QVariantHash Model::getModelParams()
{
    QVariantHash map;
    map.insert("b", m_b);   // temptation to defect
    map.insert("l", m_l);   // loner's payoff
    return map;
}

QVariantHash Model::getInspectorParams()
{
    QVariantHash map;
    // TODO
    return map;
}
