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
#include "logger.h"
#include "project.h"
#include "constants.h"
#include "utils.h"

namespace evoplex {

MainApp::MainApp()
    : m_experimentsMgr(new ExperimentsMgr())
    , m_defaultStepDelay(m_userPrefs.value("settings/stepDelay", 0).toInt())
    , m_stepsToFlush(m_userPrefs.value("settings/stepsToFlush", 10000).toInt())
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

    // load built-in plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    if (pluginsDir.cd("lib/evoplex/plugins")) {
        foreach (QString fileName, pluginsDir.entryList(QStringList("*.so"), QDir::Files)) {
            QString error;
            loadPlugin(pluginsDir.absoluteFilePath(fileName), error, false);
        }
    }
    // load user imported plugins
    QStringList plugins = m_userPrefs.value("plugins").toStringList();
    for (QString path : plugins) {
        QString error;
        loadPlugin(pluginsDir.absoluteFilePath(path), error, false);
    }
}

MainApp::~MainApp()
{
    Utils::deleteAndShrink(m_projects);
    Utils::deleteAndShrink(m_models);
    Utils::deleteAndShrink(m_graphs);
    delete m_experimentsMgr;
    m_experimentsMgr = nullptr;
}

void MainApp::setDefaultStepDelay(quint16 msec)
{
    m_defaultStepDelay = msec;
    m_userPrefs.setValue("settings/stepDelay", m_defaultStepDelay);
}

void MainApp::setStepsToFlush(int steps)
{
    m_stepsToFlush = steps;
    m_userPrefs.setValue("settings/stepsToFlush", m_stepsToFlush);
}

const AbstractPlugin* MainApp::loadPlugin(const QString& path, QString& error, const bool addToUserPrefs)
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
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)
               || !metaData.contains(PLUGIN_ATTRIBUTE_TYPE)
               || !metaData.contains(PLUGIN_ATTRIBUTE_NAME)) {
        error = QString("Unable to load the plugin at '%1'.\n"
                "Plese, make sure the following fields are not empty:\n"
                "%2, %3, %4").arg(path).arg(PLUGIN_ATTRIBUTE_UID)
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(PLUGIN_ATTRIBUTE_NAME);
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
        error = QString("Unable to load the plugin.\n'%1' must be unique! %2\n"
                "Please, choose another id or unload the plugin `%3` and try again.")
                .arg(PLUGIN_ATTRIBUTE_UID).arg(path).arg(uid);
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

    if (addToUserPrefs) {
        QStringList paths = m_userPrefs.value("plugins").toStringList();
        paths.append(path);
        m_userPrefs.setValue("plugins", paths);
    }

    emit (pluginAdded(plugin));
    qDebug() << "[MainApp] a plugin has been loaded." << path;
    return plugin;
}

bool MainApp::unloadPlugin(const AbstractPlugin* plugin, QString& error)
{
    if (!m_projects.isEmpty()) {
        error = QString("Couldn't unload the plugin `%1`.\n"
                "Please, close all projects and try again!")
                .arg(plugin->name());
        qWarning() << "[MainApp]: " << error;
        return false;
    }

    QString id = plugin->id();
    AbstractPlugin::PluginType type = plugin->type();
    if (type == AbstractPlugin::GraphPlugin && m_graphs.contains(id)) {
        delete m_graphs.take(id);
    } else if (type == AbstractPlugin::ModelPlugin && m_models.contains(id)) {
        delete m_models.take(id);
    } else {
        qFatal("[MainApp] Tried to unload a plugin (%s) which has not been loaded before.", id);
    }

    emit (pluginRemoved(id, type));
    qDebug() << "[MainApp] a plugin has been unloaded." << id;
    return true;
}

Project* MainApp::newProject(QString& error, const QString& name, const QString& dest)
{
    if (m_models.isEmpty()) {
        error = "There are no models available in the software.\n"
                "Please, make sure you import the plugins you need first!";
        qWarning() << "[MainApp] :" << error;
        return nullptr;
    } else if (m_graphs.isEmpty()) {
        error = "There are no graphs available in the software.\n"
                "Please, make sure you import the plugins you need first!";
        qWarning() << "[MainApp] :" << error;
        return nullptr;
    }

    ++m_lastProjectId;
    Project* project = new Project(this, m_lastProjectId, name, dest);
    m_projects.insert(m_lastProjectId, project);

    if (!dest.isEmpty() && !name.isEmpty()) {
        QVariantList recentProjects = m_userPrefs.value("recentProjects").toList();
        QString path = QString("%1/%2.csv").arg(dest).arg(name);
        recentProjects.removeOne(path);
        recentProjects.push_front(path);
        if (recentProjects.size() > 20) {
            recentProjects.removeLast();
        }
        m_userPrefs.setValue("recentProjects", recentProjects);
    }
    emit (projectCreated(project));
    return project;
}

void MainApp::closeProject(int projId)
{
    delete m_projects.take(projId);
}

Project* MainApp::openProject(const QString& filepath, QString& error)
{
    QFileInfo fi(filepath);
    if (!fi.isReadable() || fi.suffix() != "csv") {
        error = "Failed to open the project!\n"
                "Please, make sure it's a readable csv file!\n" + filepath;
        qWarning() << "[MainApp] :" << error;
        return nullptr;
    }

    Project* project = newProject(error, fi.baseName(), fi.absolutePath());
    if (!project) {
        return nullptr;
    } else  if (project->importExperiments(filepath, error) < 1) {
        error = "Failed to open the project!\n" + filepath
              + "Error: " + error;
        qWarning() << "[MainApp] :" << error;
        closeProject(project->id());
        return nullptr;
    }
    return project;
}

} // evoplex
