/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QJsonObject>
#include <QPluginLoader>
#include <QtDebug>

#include "core/mainapp.h"
#include "core/filemgr.h"
#include "core/project.h"
#include "utils/constants.h"
#include "utils/utils.h"

MainApp::MainApp()
    : m_fileMgr(new FileMgr(this))
    , m_trialsMgr(new TrialsMgr())
    , m_lastProjectId(-1)
{
    m_generalParamsSpace.insert(GENERAL_PARAMETER_AGENTS, "string");
    m_generalParamsSpace.insert(GENERAL_PARAMETER_GRAPHID, "string");
    m_generalParamsSpace.insert(GENERAL_PARAMETER_MODELID, "string");
    m_generalParamsSpace.insert(GENERAL_PARAMETER_SEED, "string");
    m_generalParamsSpace.insert(GENERAL_PARAMETER_STOPAT, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS));
    m_generalParamsSpace.insert(GENERAL_PARAMETER_TRIALS, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS));

    //loadModelPlugin("/home/cardinot/dev/evoplex/evoplex/build2/models/libevoplex_models.so");
}

MainApp::~MainApp()
{
    delete m_trialsMgr;
    m_trialsMgr = nullptr;
    qDeleteAll(m_projects);
    qDeleteAll(m_models);
    qDeleteAll(m_graphs);
}

bool MainApp::loadPlugin(const QString& path, QObject* instance, QJsonObject& metaData)
{
    QPluginLoader loader(path);

    metaData = loader.metaData();
    if (metaData.isEmpty()) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Couldn't find the meta data json file." << path;
        return false;
    } else if (!metaData.contains("uid")) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "'uid' cannot be empty" << path;
            return false;
    }

    QString uid = metaData["uid"].toString();
    if (m_models.contains(uid) || m_graphs.contains(uid)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "'uid' must be unique!" << path;
        return false;
    }

    instance = loader.instance();
    if (!instance) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Is it a valid .so file?" << path;
        return false;
    }

    return true;
}

const QString& MainApp::loadGraphPlugin(const QString& path)
{
    QObject* instance;
    QJsonObject metaData;
    if (!loadPlugin(path, instance, metaData)) {
        delete instance;
        return NULL;
    }

    GraphPlugin* graph = new GraphPlugin();
    graph->uid = metaData["uid"].toString();
    graph->author = metaData["author"].toString();
    graph->name = metaData["name"].toString();
    graph->description = metaData["description"].toString();
    graph->factory = qobject_cast<IPluginGraph*>(instance);

    if (metaData.contains("graphParamsSpace")) {
        QJsonObject json = metaData["graphParamsSpace"].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            graph->graphParamsSpace.insert(it.key(), it.value().toString());
        }
    }

    m_graphs.insert(graph->uid, graph);
    return graph->uid;
}

const QString& MainApp::loadModelPlugin(const QString& path)
{
    QObject* instance;
    QJsonObject metaData;
    if (!loadPlugin(path, instance, metaData)) {
        delete instance;
        return NULL;
    }

    ModelPlugin* model = new ModelPlugin();
    model->uid = metaData["uid"].toString();
    model->author = metaData["author"].toString();
    model->name = metaData["name"].toString();
    model->description = metaData["description"].toString();
    model->allowedGraphs = metaData["allowedGraphs"].toString().split(",").toVector();
    model->defaultAgentParams = Utils::minParams(model->agentParamsSpace);
    model->defaultModelParams = Utils::minParams(model->modelParamsSpace);
    model->factory = qobject_cast<IPluginModel*>(instance);

    if (metaData.contains("agentParamsSpace")) {
        QJsonObject json = metaData["agentParamsSpace"].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            model->agentParamsSpace.insert(it.key(), it.value().toString());
        }
    }

    if (metaData.contains("modelParamsSpace")) {
        QJsonObject json = metaData["modelParamsSpace"].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            model->modelParamsSpace.insert(it.key(), it.value().toString());
        }
    }

    m_models.insert(model->uid, model);
    return model->uid;
}

int MainApp::newProject(const QString& name, const QString& dir)
{
    ++m_lastProjectId;
    m_projects.insert(m_lastProjectId, new Project(this, m_lastProjectId, name, dir));
    return m_lastProjectId;
}
