/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QPluginLoader>
#include <QtDebug>

#include "core/graph.h"
#include "core/mainapp.h"
#include "core/project.h"

MainApp::MainApp()
    : m_processesMgr(new ProcessesMgr())
{
    m_generalProperties << GENERAL_PROPERTY_NAME_AGENTS
                        << GENERAL_PROPERTY_NAME_GRAPHTYPE
                        << GENERAL_PROPERTY_NAME_SEED
                        << GENERAL_PROPERTY_NAME_STOPAT;
}

MainApp::~MainApp()
{
    qDeleteAll(m_models);
    qDeleteAll(m_projects);
    delete m_processesMgr;
    m_processesMgr = NULL;
}

int MainApp::loadModel(QString path)
{
    QPluginLoader loader(path);
    QObject* obj = loader.instance();
    if (!obj) {
        qWarning() << "[MainApp] unable to load the model" << path;
        return -1;
    }

    Model* model = new Model();
    model->factory = qobject_cast<IModelFactory*>(obj);
    model->name = model->factory->name();
    model->description = model->factory->description();
    model->author = model->factory->author();
    if (model->name.isEmpty()) {
        qWarning() << "[MainApp] unable to load the model. Name cannot be empty" << path;
        return -1;
    }

    IModel* mi = model->factory->create();
    model->defaultModelParams = mi->getModelParams();
    model->defaultInspectorParams = mi->getInspectorParams();
    model->agentParamsDomain = mi->agentParamsDomain();
    delete mi;

    int modelId = m_models.size();
    m_models.insert(modelId, model);
    return modelId;
}

int MainApp::newProject(int modelId, const QString& name,
                        const QString& descr, const QString& dir)
{
    int projId = m_projects.size();
    m_projects.insert(projId, new Project(this, projId, modelId, name, descr, dir));
    return projId;
}
