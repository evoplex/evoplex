/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPluginLoader>
#include <QThread>
#include <QtDebug>

#include "mainapp.h"
#include "experimentsmgr.h"
#include "filemgr.h"
#include "project.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

MainApp::MainApp()
    : m_fileMgr(new FileMgr(this))
    , m_experimentsMgr(new ExperimentsMgr(QThread::idealThreadCount()))
    , m_lastProjectId(-1)
{
    int id = 0;
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_AGENTS, qMakePair(id++, QString("string")));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_GRAPHID, qMakePair(id++, QString("string")));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_MODELID, qMakePair(id++, QString("string")));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_SEED, qMakePair(id++, QString("int[0,%1]").arg(INT32_MAX)));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_STOPAT, qMakePair(id++, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS)));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_TRIALS, qMakePair(id++, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS)));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_AUTODELETE, qMakePair(id++, QString("bool")));
    m_generalAttrSpace.insert(OUTPUT_DIR, qMakePair(id++, QString("string")));
    m_generalAttrSpace.insert(OUTPUT_HEADER, qMakePair(id++, QString("string")));
    m_generalAttrSpace.insert(OUTPUT_AVGTRIALS, qMakePair(id++, QString("bool")));

    // load plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    if (pluginsDir.cd("lib/evoplex/plugins")) {
        foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files))
            loadPlugin(pluginsDir.absoluteFilePath(fileName));
    }
}

MainApp::~MainApp()
{
    qDeleteAll(m_projects);
    qDeleteAll(m_models);
    qDeleteAll(m_graphs);
    delete m_fileMgr;
    m_fileMgr = nullptr;
    delete m_experimentsMgr;
    m_experimentsMgr = nullptr;
}

const QString MainApp::loadPlugin(const QString& path)
{
    if (!QFile(path).exists()) {
        qWarning() << "[MainApp] unable to find the .so file." << path;
        return "";
    }

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData().value("MetaData").toObject();
    if (metaData.isEmpty()) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Couldn't find the meta data json file." << path;
        return "";
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_UID << " cannot be empty" << path;
        return "";
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_TYPE)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_TYPE << " cannot be empty" << path;
        return "";
    }

    QString type = metaData[PLUGIN_ATTRIBUTE_TYPE].toString();
    if (type != "graph" && type != "model") {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_TYPE << " must be equal to 'graph' or 'model'" << path;
        return "";
    }

    QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (m_models.contains(uid) || m_graphs.contains(uid)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_UID << " must be unique!" << path;
        return "";
    }

    QObject* instance = loader.instance(); // it'll load the plugin
    if (!instance) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Is it a valid .so file?" << path;
        loader.unload();
        return "";
    }

    if ((type == "graph" && !loadGraphPlugin(instance, metaData))
            || (type == "model" && !loadModelPlugin(instance, metaData))) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Please, check the metaData.json file.";
        loader.unload();
        return "";
    }

    qDebug() << "[MainApp] a plugin has been loaded." << path;
    return uid;
}

bool MainApp::loadGraphPlugin(QObject* instance, QJsonObject& metaData)
{
    GraphPlugin* graph = new GraphPlugin();
    graph->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    graph->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    graph->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    graph->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    graph->factory = qobject_cast<IPluginGraph*>(instance);
    graph->graphAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_GRAPHSPACE);

    if (!Utils::boundaryValues(graph->graphAttrSpace, graph->graphAttrMin, graph->graphAttrMax)) {
        return false;
    }

    if (!graph->graphAttrSpace.contains(PLUGIN_ATTRIBUTE_GRAPH_TYPE)) {
        qWarning() << "[MainApp] graph plugin -- missing 'graphType'.";
        return false;
    }

    m_graphs.insert(graph->uid, graph);
    return true;
}

bool MainApp::loadModelPlugin(QObject* instance, QJsonObject& metaData)
{
    ModelPlugin* model = new ModelPlugin();
    model->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    model->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    model->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    model->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    model->supportedGraphs = metaData[PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS].toString().split(",").toVector();
    model->customOutputs = metaData[PLUGIN_ATTRIBUTE_CUSTOMOUTPUTS].toString().split(",").toVector();
    model->factory = qobject_cast<IPluginModel*>(instance);
    model->agentAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_AGENTSPACE);
    model->edgeAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_EDGESPACE);
    model->modelAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_MODELSPACE);

    if (!Utils::boundaryValues(model->agentAttrSpace, model->agentAttrMin, model->agentAttrMax)
            || !Utils::boundaryValues(model->modelAttrSpace, model->modelAttrMin, model->modelAttrMax)) {
        return false;
    }

    m_models.insert(model->uid, model);
    return true;
}

Project* MainApp::newProject(const QString& name, const QString& dest)
{
    ++m_lastProjectId;
    Project* project = new Project(this, m_lastProjectId, name, dest);
    m_projects.insert(m_lastProjectId, project);
    return project;
}

void MainApp::closeProject(int projId)
{
    delete m_projects.take(projId);
}

Project* MainApp::openProject(const QString& filepath)
{
    QFileInfo fi(filepath);
    if (!fi.isReadable() || fi.suffix() != "csv") {
        qWarning() << "[Project] : failed to open a project!" << filepath;
        return nullptr;
    }

    Project* project = newProject(fi.baseName(), fi.absolutePath());
    if (project->importExperiments(filepath) == -1) {
        qWarning() << "[Project] : failed to open a project!" << filepath;
        closeProject(project->getId());
        return nullptr;
    }
    return project;
}

AttributesSpace MainApp::attributesSpace(const QJsonObject& metaData, const QString& name) const
{
    AttributesSpace ret;
    if (metaData.contains(name)) {
        QJsonArray json = metaData[name].toArray();
        for (int i = 0; i < json.size(); ++i) {
            QVariantMap attrs = json.at(i).toObject().toVariantMap();
            ret.insert(attrs.firstKey(), qMakePair(i, attrs.first().toString()));
        }
    }
    return ret;
}
}
