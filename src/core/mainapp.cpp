/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "mainapp.h"
#include "attributes.h"
#include "experimentsmgr.h"
#include "graphplugin.h"
#include "logger.h"
#include "modelplugin.h"
#include "plugin.h"
#include "project.h"
#include "constants.h"
#include "utils.h"
#include "../config.h"

namespace evoplex {

#if defined(Q_OS_WIN)
const char* MainApp::kPluginExtension = ".dll";
#elif defined(Q_OS_MACOS)
const char* MainApp::kPluginExtension = ".dylib";
#else
const char* MainApp::kPluginExtension = ".so";
#endif

MainApp::MainApp()
    : m_expMgr(new ExperimentsMgr()),
      m_networkMgr(new QNetworkAccessManager())
{
    qRegisterMetaType<Status>("Status"); // makes it available for signals/slots
    qRegisterMetaType<Function>("Function");

    qRegisterMetaType<PluginKey>("PluginKey");
    QMetaType::registerComparators<PluginKey>(); // allows comparing two QVariant(PluginKey)

    resetSettingsToDefault();
    m_defaultStepDelay = static_cast<quint16>(m_userPrefs.value("settings/stepDelay", m_defaultStepDelay).toInt());
    m_stepsToFlush = m_userPrefs.value("settings/stepsToFlush", m_stepsToFlush).toInt();
    m_checkUpdatesAtStart = m_userPrefs.value("settings/checkUpdatesAtStart", m_checkUpdatesAtStart).toBool();

    int id = 0;
    auto addAttrScope = [this](int& id, const QString& name, const QString& attrRangeStr) {
        m_generalAttrsScope.insert(name, AttributeRange::parse(id++, name, attrRangeStr));
    };

    addAttrScope(id, GENERAL_ATTR_EXPID, QString("int[0,%1]").arg(INT32_MAX));
    addAttrScope(id, GENERAL_ATTR_NODES, "non-empty-string");
    addAttrScope(id, GENERAL_ATTR_GRAPHID, "non-empty-string");
    addAttrScope(id, GENERAL_ATTR_MODELID, "non-empty-string");
    addAttrScope(id, GENERAL_ATTR_GRAPHVS, QString("int[0,%1]").arg(UINT16_MAX));
    addAttrScope(id, GENERAL_ATTR_MODELVS, QString("int[0,%1]").arg(UINT16_MAX));
    addAttrScope(id, GENERAL_ATTR_SEED, QString("int[0,%1]").arg(INT32_MAX));
    addAttrScope(id, GENERAL_ATTR_STOPAT, QString("int[1,%1]").arg(EVOPLEX_MAX_STEPS));
    addAttrScope(id, GENERAL_ATTR_TRIALS, QString("int[1,%1]").arg(EVOPLEX_MAX_TRIALS));
    addAttrScope(id, GENERAL_ATTR_AUTODELETE, "bool");
    addAttrScope(id, GENERAL_ATTR_GRAPHTYPE, "string");
    addAttrScope(id, GENERAL_ATTR_EDGEATTRS, "string");

    addAttrScope(id, OUTPUT_DIR, "string");
    addAttrScope(id, OUTPUT_HEADER, "string");
    // FIXME: addAttrScope(id, OUTPUT_AVGTRIALS, "bool");

    QStringList searchPaths;
    searchPaths << qApp->applicationDirPath() + "/lib/evoplex/plugins";
    searchPaths << qApp->applicationDirPath() + "/../lib/evoplex/plugins";
    for (const QString& path : searchPaths) {
        m_systemPluginsDir.setPath(path);
        if (m_systemPluginsDir.exists()) {
            break;
        }
    }

    initSystemPlugins();
    initUserPlugins();
}

MainApp::~MainApp()
{
    delete m_networkMgr;
    m_projects.clear();
    delete m_expMgr;
    m_expMgr = nullptr;
    Utils::deleteAndShrink(m_plugins);
}

void MainApp::resetSettingsToDefault()
{
    m_defaultStepDelay = 0;
    m_stepsToFlush = 10000;
    m_checkUpdatesAtStart = true;
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

void MainApp::setCheckUpdatesAtStart(bool b)
{
    m_checkUpdatesAtStart = b;
    m_userPrefs.setValue("settings/checkUpdatesAtStart", m_checkUpdatesAtStart);
}

void MainApp::initSystemPlugins()
{
    qInfo() << "searching for plugins at" << m_systemPluginsDir.absolutePath();
    const QStringList nameFilter(QString("*%1").arg(kPluginExtension));
    QStringList files = m_systemPluginsDir.entryList(nameFilter, QDir::Files);
    for (const QString& fileName : files) {
        QString error;
        loadPlugin(m_systemPluginsDir.absoluteFilePath(fileName), error, false);
    }
}

void MainApp::initUserPlugins()
{
    QStringList plugins = m_userPrefs.value("plugins").toStringList();
    QStringList::iterator it = plugins.begin();
    while (it != plugins.end()) {
        QString error;
        loadPlugin(*it, error, false);
        if (error.isEmpty()) {
            ++it;
        } else {
            it = plugins.erase(it);
        }
    }
    m_userPrefs.setValue("plugins", plugins);
}

const Plugin* MainApp::loadPlugin(const QString& path, QString& error, const bool addToUserPrefs)
{
    Plugin* plugin = Plugin::load(path, error);
    if (!plugin || plugin->type() == PluginType::Invalid) {
        delete plugin;
        return nullptr;
    }

    if (m_plugins.contains(plugin->key())) {
        error = QString("Unable to load the plugin (%1).\n"
                    "A plugin called '%2' (version %3) is already loaded.")
                    .arg(path, plugin->id()).arg(plugin->version());
        qWarning() << error;
        delete plugin;
        return nullptr;
    }

    m_plugins.insert(plugin->key(), plugin);
    if (plugin->type() == PluginType::Graph) {
        m_graphs.insert(plugin->id(), plugin->version());
    } else {
        m_models.insert(plugin->id(), plugin->version());
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

bool MainApp::unloadPlugin(const Plugin* plugin, QString& error)
{
    if (!m_projects.empty()) {
        error = QString("Couldn't unload the plugin `%1`.\n"
                "Please, close all projects and try again!")
                .arg(plugin->title());
        qWarning() << error;
        return false;
    }

    QStringList paths = m_userPrefs.value("plugins").toStringList();
    paths.removeOne(plugin->path());
    m_userPrefs.setValue("plugins", paths);

    PluginKey key = plugin->key();
    PluginType type = plugin->type();
    if (type == PluginType::Graph && m_graphs.contains(key.first)) {
        m_graphs.remove(key.first, key.second);
    } else if (type == PluginType::Model && m_models.contains(key.first)) {
        m_models.remove(key.first, key.second);
    } else {
        qFatal("Tried to unload a plugin (%s) which has not been loaded before.", qPrintable(key.first));
    }

    delete m_plugins.take(key);
    emit (pluginRemoved(key, type));
    qDebug() << "a plugin has been unloaded." << key;
    return true;
}

bool MainApp::reloadPlugin(const Plugin* plugin, QString& error)
{
    if (!m_projects.empty()) {
        error = QString("Couldn't reload the plugin `%1`.\n"
                "Please, close all projects and try again!")
                .arg(plugin->title());
        qWarning() << error;
        return false;
    }

    QString path = plugin->path();
    return unloadPlugin(plugin, error) && loadPlugin(path, error, true);
}

ProjectPtr MainApp::newProject(QString& error, const QString& filepath)
{
    if (m_projects.size() > EVOPLEX_MAX_PROJECTS) {
        error = "There are too many projects opened already!";
    } else if (m_models.isEmpty()) {
        error = "There are no models available in the software.\n"
                "Please, make sure you import the plugins you need first!";
    } else if (m_graphs.isEmpty()) {
        error = "There are no graphs available in the software.\n"
                "Please, make sure you import the plugins you need first!";
    }

    if (!error.isEmpty()) {
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
        }
        addPathToRecentProjects(filepath);
    }

    const int projectId = static_cast<int>(m_projects.size());
    auto project = std::make_shared<Project>(this, projectId);
    project->init(error, filepath);
    m_projects.insert({projectId, project});
    return project;
}

void MainApp::closeProject(int projId)
{
    auto it = m_projects.find(projId);
    if (it != m_projects.end()) {
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

ProjectPtr MainApp::project(int projId) const
{
    auto it = m_projects.find(projId);
    return (it != m_projects.end()) ? (*it).second : nullptr;
}

const GraphPlugin* MainApp::graph(const PluginKey& key) const
{
    return dynamic_cast<GraphPlugin*>(m_plugins.value(key, nullptr));
}

const ModelPlugin* MainApp::model(const PluginKey &key) const
{
    return dynamic_cast<ModelPlugin*>(m_plugins.value(key, nullptr));
}

void MainApp::checkForUpdates()
{
    QUrl url = m_userPrefs.value("settings/releasesUrl",
            qApp->organizationDomain() + "/data/releases.txt").toUrl();

    QNetworkReply* reply = m_networkMgr->get(QNetworkRequest((QUrl(url))));
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        finishedCheckingForUpdates(QJsonDocument::fromJson(reply->readAll()).object());
    });
    QNetworkRequest req(QUrl("http://www.google-analytics.com/collect"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery query("v=1&t=event&tid=UA-121617079-3&cid=evoplex");
    query.addQueryItem("ec", qApp->applicationVersion());
    query.addQueryItem("ea", QSysInfo::prettyProductName());
    m_networkMgr->post(req, query.toString().toLatin1());
}

void MainApp::finishedCheckingForUpdates(const QJsonObject& j)
{
    if (j.isEmpty()) {
        qWarning() << "failed to check for updates (connection failed)";
        return;
    }

    QDate d(2000, 1, 1);
    for (const QString& s : j.keys()) {
        auto _d = QDate::fromString(s, "yyyy-MM-dd");
        if (_d.isValid() && _d > d) {
            d = _d;
        }
    }

    if (d == QDate(2000, 1, 1)) {
        qWarning() << "failed to check for updates (invalid database: keys not found)";
        return;
    }

    auto mostRecent = j.value(d.toString("yyyy-MM-dd")).toObject();
    QStringList currVersion = QString(EVOPLEX_VERSION).split(".");
    if (currVersion.size() != 3 || !mostRecent.contains("major") ||
            !mostRecent.contains("minor") || !mostRecent.contains("patch")) {
        qWarning() << "failed to check for updates (invalid database: missing fields)";
        return;
    }

    if (mostRecent.value("major").toInt() > currVersion.at(0).toInt() ||
        mostRecent.value("minor").toInt() > currVersion.at(1).toInt() ||
        mostRecent.value("patch").toInt() > currVersion.at(2).toInt())
    {
        emit (newVersionAvailable(mostRecent.toVariantMap()));
    }
}

} // evoplex
