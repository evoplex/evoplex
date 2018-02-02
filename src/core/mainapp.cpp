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
#include <QSettings>
#include <QThread>
#include <QtDebug>

#include "mainapp.h"
#include "experimentsmgr.h"
#include "project.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

MainApp::MainApp()
    : m_experimentsMgr(new ExperimentsMgr())
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

    QSettings s;
    m_defaultDelay = s.value("settings/delay", 0).toInt();

    // load built-in plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    if (pluginsDir.cd("lib/evoplex/plugins")) {
        foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files)) {
            QString error;
            loadPlugin(pluginsDir.absoluteFilePath(fileName), error);
        }
    }
    // load user imported plugins
    QStringList plugins = s.value("plugins").toStringList();
    for (QString path : plugins) {
        QString error;
        loadPlugin(pluginsDir.absoluteFilePath(path), error);
    }
}

MainApp::~MainApp()
{
    qDeleteAll(m_projects);
    qDeleteAll(m_models);
    qDeleteAll(m_graphs);
    delete m_experimentsMgr;
    m_experimentsMgr = nullptr;
}

const AbstractPlugin* MainApp::importPlugin(const QString& path, QString& error)
{
    const AbstractPlugin* plugin = loadPlugin(path, error);
    if (plugin) {
        QSettings s;
        QStringList paths = s.value("plugins").toStringList();
        paths.append(path);
        s.setValue("plugins", paths);
    }
    return plugin;
}

const AbstractPlugin* MainApp::loadPlugin(const QString& path, QString& error)
{
    if (!QFile(path).exists()) {
        error = "Unable to find the .so file. " + path;
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData().value("MetaData").toObject();
    if (metaData.isEmpty()) {
        error = "Unable to load the plugin.\nWe couldn't find the meta data for this plugin.\n" + path;
        qWarning() << "[MainApp] " << error;
        return nullptr;
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)) {
        error = QString("Unable to load the plugin.\n'%1' cannot be empty. %2")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(path);
        qWarning() << "[MainApp] " << error;
        return nullptr;
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_TYPE)) {
        error = QString("Unable to load the plugin.\n'%1' cannot be empty. %2")
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(path);
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    QString type = metaData[PLUGIN_ATTRIBUTE_TYPE].toString();
    if (type != "graph" && type != "model") {
        error = QString("Unable to load the plugin.\n'%1' must be equal to 'graph' or 'model'. %2")
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(path);
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (m_models.contains(uid) || m_graphs.contains(uid)) {
        error = QString("Unable to load the plugin.\n'%1' must be unique! %2")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(path);
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    QObject* instance = loader.instance(); // it'll load the plugin
    if (!instance) {
        error = QString("Unable to load the plugin.\nIs it a valid .so file?\n %1").arg(path);
        loader.unload();
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    AbstractPlugin* plugin = nullptr;
    if (type == "graph") {
        GraphPlugin* graph = new GraphPlugin(instance, &metaData);
        if (graph->isValid()) {
            m_graphs.insert(graph->id(), graph);
            plugin = graph;
        }
    } else {
        ModelPlugin* model = new ModelPlugin(instance, &metaData);
        if (model->isValid()) {
            m_models.insert(model->id(), model);
            plugin = model;
        }
    }

    if (!plugin->isValid()) {
        error = QString("Unable to load the plugin.\nPlease, check the metaData.json file.\n %1").arg(path);
        loader.unload();
        qWarning() << "[MainApp] " << error;
        return nullptr;
    }

    qDebug() << "[MainApp] a plugin has been loaded." << path;
    return plugin;
}

Project* MainApp::newProject(const QString& name, const QString& dest)
{
    ++m_lastProjectId;
    Project* project = new Project(this, m_lastProjectId, name, dest);
    m_projects.insert(m_lastProjectId, project);

    if (!dest.isEmpty() && !name.isEmpty()) {
        QSettings s;
        QVariantList recentProjects = s.value("recentProjects").toList();
        QString path = QString("%1/%2.csv").arg(dest).arg(name);
        recentProjects.removeOne(path);
        recentProjects.push_front(path);
        if (recentProjects.size() > 20) {
            recentProjects.removeLast();
        }
        s.setValue("recentProjects", recentProjects);
    }
    emit (projectCreated(project));
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
        closeProject(project->id());
        return nullptr;
    }
    return project;
}

} // evoplex
