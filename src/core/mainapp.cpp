/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QPluginLoader>
#include <QtDebug>

#include "core/mainapp.h"
#include "core/experimentsmgr.h"
#include "core/filemgr.h"
#include "core/project.h"
#include "utils/constants.h"
#include "utils/utils.h"

MainApp::MainApp()
    : m_fileMgr(new FileMgr(this))
    , m_experimentsMgr(new ExperimentsMgr())
    , m_lastProjectId(-1)
{
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_AGENTS, "string");
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_GRAPHID, "string");
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_MODELID, "string");
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_SEED, "string");
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_STOPAT, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS));
    m_generalAttrSpace.insert(GENERAL_ATTRIBUTE_TRIALS, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS));

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
        return "";
    }

    GraphPlugin* graph = new GraphPlugin();
    graph->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    graph->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    graph->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    graph->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    graph->factory = qobject_cast<IPluginGraph*>(instance);

    if (metaData.contains(PLUGIN_ATTRIBUTE_GRAPHSPACE)) {
        QJsonObject json = metaData[PLUGIN_ATTRIBUTE_GRAPHSPACE].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            // let's add the uid to the key just to avoid clashes
            QString key = QString("%1_%2").arg(graph->uid).arg(it.key());
            graph->graphAttrSpace.insert(key, it.value().toString());
        }
    }

    Utils::boundaryValues(graph->graphAttrSpace, graph->graphAttrMin, graph->graphAttrMax);
    m_graphs.insert(graph->uid, graph);
    return graph->uid;
}

QString MainApp::loadModelPlugin(const QString& path)
{
    QObject* instance = nullptr;
    QJsonObject metaData;
    if (!loadPlugin(path, &instance, metaData)) {
        return "";
    }

    ModelPlugin* model = new ModelPlugin();
    model->uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    model->author = metaData[PLUGIN_ATTRIBUTE_AUTHOR].toString();
    model->name = metaData[PLUGIN_ATTRIBUTE_NAME].toString();
    model->description = metaData[PLUGIN_ATTRIBUTE_DESCRIPTION].toString();
    model->supportedGraphs = metaData[PLUGIN_ATTRIBUTE_SUPPORTEDGRAPHS].toString().split(",").toVector();
    model->factory = qobject_cast<IPluginModel*>(instance);

    if (metaData.contains(PLUGIN_ATTRIBUTE_AGENTSPACE)) {
        QJsonObject json = metaData[PLUGIN_ATTRIBUTE_AGENTSPACE].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            model->agentAttrSpace.insert(it.key(), it.value().toString());
        }
    }

    if (metaData.contains(PLUGIN_ATTRIBUTE_MODELSPACE)) {
        QJsonObject json = metaData[PLUGIN_ATTRIBUTE_MODELSPACE].toObject();
        for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
            // let's add the uid to the key just to avoid clashes
            QString key = QString("%1_%2").arg(model->uid).arg(it.key());
            model->modelAttrSpace.insert(key, it.value().toString());
        }
    }

    Utils::boundaryValues(model->agentAttrSpace, model->agentAttrMin, model->agentAttrMax);
    Utils::boundaryValues(model->modelAttrSpace, model->modelAttrMin, model->modelAttrMax);
    m_models.insert(model->uid, model);
    return model->uid;
}

int MainApp::newProject(const QString& name, const QString& dir)
{
    ++m_lastProjectId;
    m_projects.insert(m_lastProjectId, new Project(this, m_lastProjectId, name, dir));
    return m_lastProjectId;
}
