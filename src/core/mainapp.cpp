/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QJsonObject>
#include <QPluginLoader>
#include <QtDebug>

#include "core/mainapp.h"
#include "core/project.h"
#include "utils/utils.h"

MainApp::MainApp(): m_processesMgr(new ProcessesMgr())
{
    m_generalProperties << GENERAL_PARAMETER_AGENTS
                        << GENERAL_PARAMETER_GRAPHID
                        << GENERAL_PARAMETER_SEED
                        << GENERAL_PARAMETER_STOPAT;

    //loadModelPlugin("/home/cardinot/dev/evoplex/evoplex/build2/models/libevoplex_models.so");
}

MainApp::~MainApp()
{
    qDeleteAll(m_projects);
    qDeleteAll(m_models);
    qDeleteAll(m_graphs);
    delete m_processesMgr;
    m_processesMgr = NULL;
}

const QString& MainApp::loadGraphPlugin(QString path)
{
    QPluginLoader loader(path);
    QJsonObject json = loader.metaData();
    QObject* obj = loader.instance();

    if (json.isEmpty() || !obj) {
        qWarning() << "[MainApp] unable to load the graph" << path;
        return NULL;
    } else if (!json.contains("uid")) {
        qWarning() << "[MainApp] unable to load the graph."
                   << "'uid' cannot be empty" << path;
        delete obj;
        return NULL;
    } else if (m_models.contains(json["uid"].toString())) {
        qWarning() << "[MainApp] unable to load the graph."
                   << "'uid' must be unique!" << path;
        delete obj;
        return NULL;
    }

    GraphPlugin* graph = new GraphPlugin();
    graph->uid = json["uid"].toString();
    graph->author = json["author"].toString();
    graph->name = json["name"].toString();
    graph->description = json["description"].toString();
    graph->graphParamsSpace = json["graphParamsSpace"].toObject().toVariantHash();
    graph->factory = qobject_cast<IPluginGraph*>(obj);
    m_graphs.insert(graph->uid, graph);

    return graph->uid;
}

const QString& MainApp::loadModelPlugin(QString path)
{
    QPluginLoader loader(path);
    QJsonObject json = loader.metaData();
    QObject* obj = loader.instance();

    if (json.isEmpty() || !obj) {
        qWarning() << "[MainApp] unable to load the model" << path;
        return NULL;
    } else if (!json.contains("uid")) {
        qWarning() << "[MainApp] unable to load the model."
                   << "'uid' cannot be empty" << path;
        delete obj;
        return NULL;
    } else if (m_models.contains(json["uid"].toString())) {
        qWarning() << "[MainApp] unable to load the graph."
                   << "'uid' must be unique!" << path;
        delete obj;
        return NULL;
    }

    // lets deny the properties which might be used by the GUI
    QVariantHash agentParamsSpace = json["agentParamsSpace"].toObject().toVariantHash();
    QStringList reservedParams;
    reservedParams << "id" << "x" << "y" << "z";
    foreach (QString r, reservedParams) {
        if (agentParamsSpace.contains(r)) {
            qWarning() << "[MainApp] unable to load the model."
                       << "The agent properties 'id', 'x', 'y' and 'z' are reserved and must not be used in the model."
                       << path;
            delete obj;
            return NULL;
        }
    }

    ModelPlugin* model = new ModelPlugin();
    model->uid = json["uid"].toString();
    model->author = json["author"].toString();
    model->name = json["name"].toString();
    model->description = json["description"].toString();
    model->agentParamsSpace = agentParamsSpace;
    model->modelParamsSpace = json["modelParamsSpace"].toObject().toVariantHash();
    model->defaultAgentParams = Utils::minParams(model->agentParamsSpace);
    model->defaultModelParams = Utils::minParams(model->modelParamsSpace);
    model->factory = qobject_cast<IPluginModel*>(obj);
    m_models.insert(model->uid, model);

    return model->uid;
}

int MainApp::newProject(const QString& modelId, const QString& name,
                        const QString& descr, const QString& dir)
{
    int projId = m_projects.size();
    m_projects.insert(projId, new Project(this, projId, modelId, name, descr, dir));
    return projId;
}
