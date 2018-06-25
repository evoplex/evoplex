/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#if defined(Q_OS_WIN)
const char* MainApp::kPluginExtension = ".dll";
#elif defined(Q_OS_MACOS)
const char* MainApp::kPluginExtension = ".dylib";
#else
const char* MainApp::kPluginExtension = ".so";
#endif

MainApp::MainApp()
    : m_experimentsMgr(new ExperimentsMgr())
{
    resetSettingsToDefault();
    m_defaultStepDelay = m_userPrefs.value("settings/stepDelay", m_defaultStepDelay).toInt();
    m_stepsToFlush = m_userPrefs.value("settings/stepsToFlush", m_stepsToFlush).toInt();

    int id = 0;
    auto addAttrScope = [this](int& id, const QString& name, const QString& attrRangeStr) {
        m_generalAttrsScope.insert(name, AttributeRange::parse(id++, name, attrRangeStr));
    };

    addAttrScope(id, GENERAL_ATTRIBUTE_EXPID, QString("int[0,%1]").arg(INT32_MAX));
    addAttrScope(id, GENERAL_ATTRIBUTE_NODES, "string");
    addAttrScope(id, GENERAL_ATTRIBUTE_GRAPHID, "string");
    addAttrScope(id, GENERAL_ATTRIBUTE_MODELID, "string");
    addAttrScope(id, GENERAL_ATTRIBUTE_SEED, QString("int[0,%1]").arg(INT32_MAX));
    addAttrScope(id, GENERAL_ATTRIBUTE_STOPAT, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS));
    addAttrScope(id, GENERAL_ATTRIBUTE_TRIALS, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS));
    addAttrScope(id, GENERAL_ATTRIBUTE_AUTODELETE, "bool");
    addAttrScope(id, GENERAL_ATTRIBUTE_GRAPHTYPE, "string");

    addAttrScope(id, OUTPUT_DIR, "string");
    addAttrScope(id, OUTPUT_HEADER, "string");
    addAttrScope(id, OUTPUT_AVGTRIALS, "bool");

    // load built-in plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath() + "/../lib/evoplex/plugins");
    qDebug() << "searching for plugins at" << pluginsDir.absolutePath();
    if (pluginsDir.exists()) {
        const QStringList nameFilter(QString("*%1").arg(kPluginExtension));
        QStringList files = pluginsDir.entryList(nameFilter, QDir::Files);
        for (const QString& fileName : files) {
            QString error;
            loadPlugin(pluginsDir.absoluteFilePath(fileName), error, false);
        }
    }
    // load user imported plugins
    QStringList plugins = m_userPrefs.value("plugins").toStringList();
    QStringList::iterator it = plugins.begin();
    while (it != plugins.end()) {
        QString error;
        loadPlugin(pluginsDir.absoluteFilePath(*it), error, false);
        if (error.isEmpty()) {
            ++it;
        } else {
            it = plugins.erase(it);
        }
    }
    m_userPrefs.setValue("plugins", plugins);
}

MainApp::~MainApp()
{
    for (auto& p : m_projects) {
        p.second->destroyExperiments();
    }
    Utils::deleteAndShrink(m_models);
    Utils::deleteAndShrink(m_graphs);
    delete m_experimentsMgr;
    m_experimentsMgr = nullptr;
}

void MainApp::resetSettingsToDefault()
{
    m_defaultStepDelay = 0;
    m_stepsToFlush = 10000;
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
        error = "Unable to find the file. " + path;
        qWarning() << error;
        return nullptr;
    }

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData().value("MetaData").toObject();
    if (metaData.isEmpty()) {
        error = "Unable to load the plugin.\nWe couldn't find the meta data for this plugin.\n" + path;
        qWarning() << error;
        return nullptr;
    } else if (!metaData.contains(PLUGIN_ATTRIBUTE_UID)
               || !metaData.contains(PLUGIN_ATTRIBUTE_TYPE)
               || !metaData.contains(PLUGIN_ATTRIBUTE_NAME)) {
        error = QString("Unable to load the plugin at '%1'.\n"
                "Plese, make sure the following fields are not empty:\n"
                "%2, %3, %4").arg(path).arg(PLUGIN_ATTRIBUTE_UID)
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(PLUGIN_ATTRIBUTE_NAME);
        qWarning() << error;
        return nullptr;
    }

    QString type = metaData[PLUGIN_ATTRIBUTE_TYPE].toString();
    if (type != "graph" && type != "model") {
        error = QString("Unable to load the plugin.\n'%1' must be equal to 'graph' or 'model'. %2")
                .arg(PLUGIN_ATTRIBUTE_TYPE).arg(path);
        qWarning() << error;
        return nullptr;
    }

    QString uid = metaData[PLUGIN_ATTRIBUTE_UID].toString();
    if (uid.contains("_")) {
        error = QString("Unable to load the plugin (%1).\n"
                        "The '%2:%3' should not have the underscore symbol.\n"
                        "Please, fix this id and try again.")
                        .arg(path).arg(PLUGIN_ATTRIBUTE_UID).arg(uid);
        qWarning() << error;
        return nullptr;
    } else if (m_models.contains(uid) || m_graphs.contains(uid)) {
        error = QString("Unable to load the plugin (%1).\n"
                        "The %2 '%3' is already being used by another plugin.\n"
                        "Please, unload the plugin '%4' (or choose another id) and try again.")
                        .arg(path).arg(PLUGIN_ATTRIBUTE_UID).arg(uid).arg(uid);
        qWarning() << error;
        return nullptr;
    }

    QObject* instance = loader.instance(); // it'll load the plugin
    if (!instance) {
        error = QString("Unable to load the plugin.\n"
                        "Please, make sure it is a valid Evoplex plugin.\n %1").arg(path);
        loader.unload();
        qWarning() << error;
        return nullptr;
    }

    AbstractPlugin* plugin = nullptr;
    if (type == "graph") {
        plugin = new GraphPlugin(instance, &metaData, path);
        if (plugin->isValid()) {
            m_graphs.insert(plugin->id(), dynamic_cast<GraphPlugin*>(plugin));
        }
    } else {
        plugin = new ModelPlugin(instance, &metaData, path);
        if (plugin->isValid()) {
            m_models.insert(plugin->id(), dynamic_cast<ModelPlugin*>(plugin));
        }
    }

    if (!plugin || !plugin->isValid()) {
        error = QString("Unable to load the plugin.\n"
                        "Please, check the metaData.json file.\n %1").arg(path);
        loader.unload();
        qWarning() << error;
        delete plugin;
        return nullptr;
    }

    if (addToUserPrefs) {
        QStringList paths = m_userPrefs.value("plugins").toStringList();
        paths.append(path);
        m_userPrefs.setValue("plugins", paths);
    }

    emit (pluginAdded(plugin));
    qDebug() << "a plugin has been loaded." << path;
    return plugin;
}

bool MainApp::unloadPlugin(const AbstractPlugin* plugin, QString& error)
{
    if (!m_projects.empty()) {
        error = QString("Couldn't unload the plugin `%1`.\n"
                "Please, close all projects and try again!")
                .arg(plugin->name());
        qWarning() << error;
        return false;
    }

    QStringList paths = m_userPrefs.value("plugins").toStringList();
    paths.removeOne(plugin->path());
    m_userPrefs.setValue("plugins", paths);

    QString id = plugin->id();
    AbstractPlugin::PluginType type = plugin->type();
    if (type == AbstractPlugin::GraphPlugin && m_graphs.contains(id)) {
        delete m_graphs.take(id);
    } else if (type == AbstractPlugin::ModelPlugin && m_models.contains(id)) {
        delete m_models.take(id);
    } else {
        qFatal("Tried to unload a plugin (%s) which has not been loaded before.", qPrintable(id));
    }

    emit (pluginRemoved(id, type));
    qDebug() << "a plugin has been unloaded." << id;
    return true;
}

ProjectSP MainApp::newProject(QString& error, const QString& filepath)
{
    if (m_projects.size() > EVOPLEX_MAX_PROJECTS) {
        error = "There are too many opened projects already!";
        qWarning() << error;
        return nullptr;
    } else if (m_models.isEmpty()) {
        error = "There are no models available in the software.\n"
                "Please, make sure you import the plugins you need first!";
        qWarning() << error;
        return nullptr;
    } else if (m_graphs.isEmpty()) {
        error = "There are no graphs available in the software.\n"
                "Please, make sure you import the plugins you need first!";
        qWarning() << error;
        return nullptr;
    }

    if (!filepath.isEmpty()) {
        QFileInfo fi(filepath);
        if (!fi.isReadable() || fi.suffix() != "csv") {
            error = "Failed to open the project!\n"
                    "Please, make sure it's a readable csv file!\n" + filepath;
            qWarning() << error;
            return nullptr;
        } else {
            addPathToRecentProjects(filepath);
        }
    }

    const int projectId = static_cast<int>(m_projects.size());
    ProjectSP project = ProjectSP::create(this, projectId);
    if (!project->init(error, filepath)) {
        return nullptr;
    }

    m_projects.insert({projectId, project});
    return project;
}

void MainApp::closeProject(int projId)
{
    std::map<int, ProjectSP>::iterator it = m_projects.find(projId);
    if (it != m_projects.end()) {
        (*it).second->destroyExperiments();
        (*it).second.clear();
        m_projects.erase(it);
    }
}

void MainApp::addPathToRecentProjects(const QString& projectFilePath)
{
    QVariantList recentProjects = m_userPrefs.value("recentProjects").toList();
    recentProjects.removeOne(projectFilePath);
    recentProjects.push_front(projectFilePath);
    if (recentProjects.size() > 20) {
        recentProjects.removeLast();
    }
    m_userPrefs.setValue("recentProjects", recentProjects);
    emit (listOfRecentProjectsUpdated());
}

} // evoplex
