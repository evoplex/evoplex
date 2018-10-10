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

#ifndef MAINAPP_H
#define MAINAPP_H

#include <map>
#include <memory>

#include <QDir>
#include <QHash>
#include <QObject>
#include <QSettings>

#include "attributerange.h"
#include "enum.h"

class QNetworkAccessManager;

namespace evoplex {

class ExperimentsMgr;
class GraphPlugin;
class ModelPlugin;
class Project;
class Plugin;

using PluginKey = std::pair<QString, quint16>;  // <id, version>
using ProjectPtr = std::shared_ptr<Project>;
using ProjectWPtr = std::weak_ptr<Project>;

class MainApp : public QObject
{
    Q_OBJECT

public:
    static const char* kPluginExtension;

    explicit MainApp();
    ~MainApp();

    void resetSettingsToDefault();

    // load plugin from a .so file; return nullptr if unsuccessful
    const Plugin* loadPlugin(const QString& path, QString& error, const bool addToUserPrefs);

    // unload an existing plugin; return true if successful
    bool unloadPlugin(const Plugin* plugin, QString& error);

    // reload an existing plugin; return true if successful
    bool reloadPlugin(const Plugin* plugin, QString& error);

    // Create a new project
    ProjectPtr newProject(QString &error, const QString& filepath="");

    void closeProject(int projId);

    inline quint16 defaultStepDelay() const;
    void setDefaultStepDelay(quint16 msec);

    inline int stepsToFlush() const;
    void setStepsToFlush(int steps);

    inline bool checkUpdatesAtStart() const;
    void setCheckUpdatesAtStart(bool b);

    inline ExperimentsMgr* expMgr() const;
    inline const QHash<PluginKey, Plugin*>& plugins() const;
    inline const QMultiHash<QString, quint16>& graphs() const;
    inline const QMultiHash<QString, quint16>& models() const;
    inline const std::map<int, ProjectPtr>& projects() const;

    const GraphPlugin* graph(const PluginKey &key) const;
    const ModelPlugin* model(const PluginKey& key) const;
    ProjectPtr project(int projId) const;

    inline const AttributesScope& generalAttrsScope() const;

    void addPathToRecentProjects(const QString& projectFilePath);

public slots:
    /**
     * @brief Checks if a newer version of Evoplex was released.
     * @see checkedForUpdates
     */
    void checkForUpdates();

signals:
    /**
     * @brief This signal is emitted when a successful checkForUpdates() call ends.
     * @param[out] data A map with the info about the newest version available.
     */
    void newVersionAvailable(const QVariantMap& data);
    void pluginAdded(const Plugin* plugin);
    void pluginRemoved(PluginKey key, PluginType t);
    void listOfRecentProjectsUpdated();

private:
    /**
     * @brief Triggered when a checkForUpdates() call ends.
     * @param j The json object with the 'releases.txt' data
     *          obtained from the internet.
     * If there are updates available, this function will emit
     * the newVersionAvailable() signal.
     */
    void finishedCheckingForUpdates(const QJsonObject& j);

    /**
     * @brief Loads built-in plugins.
     */
    void initSystemPlugins();

    /**
     * @brief Loads user imported plugins.
     * It removes failed plugins from the list.
     */
    void initUserPlugins();

    ExperimentsMgr* m_expMgr;
    QDir m_systemPluginsDir;

    QSettings m_userPrefs;
    quint16 m_defaultStepDelay; // msec
    int m_stepsToFlush;
    bool m_checkUpdatesAtStart;

    QNetworkAccessManager* m_networkMgr;

    std::map<int, ProjectPtr> m_projects; // opened projects.

    QHash<PluginKey, Plugin*> m_plugins;   // loaded plugins
    QMultiHash<QString, quint16> m_graphs; // versions of each loaded graphIds (each key can have multiple values)
    QMultiHash<QString, quint16> m_models; // versions of each loaded modelIds (each key can have multiple values)

    // let's build a hash with the name and attrRange of the essential attributes
    // it is important to validate the contents of csv files
    AttributesScope m_generalAttrsScope;
};

/************************************************************************
   MainApp: Inline member functions
 ************************************************************************/

inline quint16 MainApp::defaultStepDelay() const
{ return m_defaultStepDelay; }

inline int MainApp::stepsToFlush() const
{ return m_stepsToFlush; }

inline bool MainApp::checkUpdatesAtStart() const
{ return m_checkUpdatesAtStart; }

inline ExperimentsMgr* MainApp::expMgr() const
{ return m_expMgr; }

inline const QHash<PluginKey, Plugin*>& MainApp::plugins() const
{ return m_plugins; }

inline const QMultiHash<QString, quint16>& MainApp::graphs() const
{ return m_graphs; }

inline const QMultiHash<QString, quint16>& MainApp::models() const
{ return m_models; }

inline const std::map<int, ProjectPtr>& MainApp::projects() const
{ return m_projects; }

inline const AttributesScope& MainApp::generalAttrsScope() const
{ return m_generalAttrsScope; }

} // evoplex
#endif // MAINAPP_H
