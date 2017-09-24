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

    // load plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("plugins");
    if (pluginsDir.cd("graphs")) {
        foreach (QString dir, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            pluginsDir.cd(dir);
            foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files)) {
                loadGraphPlugin(pluginsDir.absoluteFilePath(fileName));
            }
            pluginsDir.cdUp();
        }
        pluginsDir.cdUp();
    }
    if (pluginsDir.cd("models")) {
        foreach (QString dir, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            pluginsDir.cd(dir);
            foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files)) {
                loadModelPlugin(pluginsDir.absoluteFilePath(fileName));
            }
            pluginsDir.cdUp();
        }
    }
}

MainApp::~MainApp()
{
    delete m_fileMgr;
    m_fileMgr = nullptr;
    delete m_experimentsMgr;
    m_experimentsMgr = nullptr;
    qDeleteAll(m_projects);
    qDeleteAll(m_models);
    qDeleteAll(m_graphs);
}

bool MainApp::loadPlugin(const QString& path, QObject** instance, QJsonObject& metaData)
{
    if (!QFile(path).exists()) {
        qWarning() << "[MainApp] unable to find the .so file." << path;
        return false;
    }

    QPluginLoader loader(path);
    metaData = loader.metaData().value("MetaData").toObject();
    if (metaData.isEmpty()) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Couldn't find the meta data json file." << path;
        return false;
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_UID << " cannot be empty" << path;
            return false;
    }

    QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (m_models.contains(uid) || m_graphs.contains(uid)) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << PLUGIN_ATTRIBUTE_UID << " must be unique!" << path;
        return false;
    }

    *instance = loader.instance();
    if (!instance) {
        qWarning() << "[MainApp] unable to load the plugin."
                   << "Is it a valid .so file?" << path;
        return false;
    }

    qDebug() << "[MainApp] a plugin has been loaded." << path;
    return true;
}

QString MainApp::loadGraphPlugin(const QString& path)
{
    QObject* instance = nullptr;
    QJsonObject metaData;
    if (!loadPlugin(path, &instance, metaData)) {
        instance->deleteLater();
        return "";
    }

    GraphPlugin* graph = new GraphPlugin();
    graph->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    graph->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    graph->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    graph->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    graph->factory = qobject_cast<IPluginGraph*>(instance);
    graph->graphAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_GRAPHSPACE);

    if (!Utils::boundaryValues(graph->graphAttrSpace, graph->graphAttrMin, graph->graphAttrMax)) {
        instance->deleteLater();
        return "";
    }

    m_graphs.insert(graph->uid, graph);
    return graph->uid;
}

QString MainApp::loadModelPlugin(const QString& path)
{
    QObject* instance = nullptr;
    QJsonObject metaData;
    if (!loadPlugin(path, &instance, metaData)) {
        instance->deleteLater();
        return "";
    }

    ModelPlugin* model = new ModelPlugin();
    model->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    model->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    model->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    model->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    model->supportedGraphs = metaData[PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS].toString().split(",").toVector();
    model->factory = qobject_cast<IPluginModel*>(instance);
    model->agentAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_AGENTSPACE);
    model->modelAttrSpace = attributesSpace(metaData, PLUGIN_ATTRIBUTE_MODELSPACE);

    if (!Utils::boundaryValues(model->agentAttrSpace, model->agentAttrMin, model->agentAttrMax)
            || !Utils::boundaryValues(model->modelAttrSpace, model->modelAttrMin, model->modelAttrMax)) {
        instance->deleteLater();
        return "";
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
