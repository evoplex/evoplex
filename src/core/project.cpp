/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "project.h"

Project::Project(const MainApp& mainapp, MainApp::Model *model,
        const QString& name, const QString& descr, const QString& dir)
    : m_mainApp(mainapp)
    , m_model(model)
    , m_name(name)
    , m_description(descr)
    , m_dir(dir)
{
}

Project::~Project()
{
}

Simulation* Project::newSimulation(Graph* graph, QList<QMetaProperty> params)
{
    IModel* mi = m_model->factory->create();
    mi->init(graph, params);
    return new Simulation(mi);
}
