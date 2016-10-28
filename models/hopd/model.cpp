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

void Model::init(Graph *graph, QVariantMap params)
{
}

void Model::algorithmStep()
{
    qDebug() << "HOPD";
}

