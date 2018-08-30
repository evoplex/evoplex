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

#ifndef MAINAPP_H
#define MAINAPP_H

#include <map>
#include <memory>

#include <QDir>
#include <QHash>
#include <QtNetwork/QNetworkAccessManager>
#include <QObject>
#include <QSettings>

#include "attributerange.h"
#include "enum.h"

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
    // checks for updates
    // returns an empty QJsonObject if there's no updates
    void checkForUpdates();

signals:
    void pluginAdded(const Plugin* plugin);
    void pluginRemoved(PluginKey key, PluginType t);
    void listOfRecentProjectsUpdated();
    void checkedForUpdates(const QJsonObject& json);

private:
    // Load built-in plugins
    void initSystemPlugins();

    // Load user imported plugins.
    // It removes failed plugins from the list
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
