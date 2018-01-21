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
    auto addAttrSpace = [this](int& id, const QString& name, const QString& space) {
        m_generalAttrSpace.insert(name, ValueSpace::parse(id++, name, space));
    };

    addAttrSpace(id, GENERAL_ATTRIBUTE_AGENTS, "string");
    addAttrSpace(id, GENERAL_ATTRIBUTE_GRAPHID, "string");
    addAttrSpace(id, GENERAL_ATTRIBUTE_MODELID, "string");
    addAttrSpace(id, GENERAL_ATTRIBUTE_SEED, QString("int[0,%1]").arg(INT32_MAX));
    addAttrSpace(id, GENERAL_ATTRIBUTE_STOPAT, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS));
    addAttrSpace(id, GENERAL_ATTRIBUTE_TRIALS, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS));
    addAttrSpace(id, GENERAL_ATTRIBUTE_AUTODELETE, "bool");
    addAttrSpace(id, GENERAL_ATTRIBUTE_GRAPHTYPE, "string");

    addAttrSpace(id, OUTPUT_DIR, "string");
    addAttrSpace(id, OUTPUT_HEADER, "string");
    addAttrSpace(id, OUTPUT_AVGTRIALS, "bool");

    // load plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    if (pluginsDir.cd("lib/evoplex/plugins")) {
        foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files)) {
            QString error;
            loadPlugin(pluginsDir.absoluteFilePath(fileName), error);
        }
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

const QString MainApp::loadPlugin(const QString& path, QString& error)
{
    if (!QFile(path).exists()) {
        error = "Unable to find the .so file. " + path;
        qWarning() << "[MainApp] " << error;
        return "";
    }

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData().value("MetaData").toObject();
    if (metaData.isEmpty()) {
        error = "Unable to load the plugin.\nWe couldn't find the meta data for this plugin.\n" + path;
        qWarning() << "[MainApp] " << error;
        return "";
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)) {
        error = QString("Unable to load the plugin.\n'%1' cannot be empty. %2")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(path);
        qWarning() << "[MainApp] " << error;
        return "";
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_TYPE)) {
        error = QString("Unable to load the plugin.\n'%1' cannot be empty. %2")
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(path);
        qWarning() << "[MainApp] " << error;
        return "";
    }

    QString type = metaData[PLUGIN_ATTRIBUTE_TYPE].toString();
    if (type != "graph" && type != "model") {
        error = QString("Unable to load the plugin.\n'%1' must be equal to 'graph' or 'model'. %2")
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(path);
        qWarning() << "[MainApp] " << error;
        return "";
    }

    QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (m_models.contains(uid) || m_graphs.contains(uid)) {
        error = QString("Unable to load the plugin.\n'%1' must be unique! %2")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(path);
        qWarning() << "[MainApp] " << error;
        return "";
    }

    QObject* instance = loader.instance(); // it'll load the plugin
    if (!instance) {
        error = QString("Unable to load the plugin.\nIs it a valid .so file?\n %1").arg(path);
        qWarning() << "[MainApp] " << error;
        loader.unload();
        return "";
    }

    bool isValid = false;
    if (type == "graph") {
        GraphPlugin* plugin = new GraphPlugin(instance, &metaData);
        if (plugin->isValid()) {
            m_graphs.insert(plugin->id(), plugin);
            isValid = true;
        }
    } else {
        ModelPlugin* plugin = new ModelPlugin(instance, &metaData);
        if (plugin->isValid()) {
            m_models.insert(plugin->id(), plugin);
            isValid = true;
        }
    }

    if (!isValid) {
        error = QString("Unable to load the plugin.\nPlease, check the metaData.json file.\n %1").arg(path);
        qWarning() << "[MainApp] " << error;
        loader.unload();
        return "";
    }

    qDebug() << "[MainApp] a plugin has been loaded." << path;
    return uid;
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

} // evoplex
