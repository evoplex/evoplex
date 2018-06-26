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

#include <QDir>
#include <QHash>
#include <QObject>
#include <QSettings>
#include <memory>

#include "plugininterfaces.h"
#include "attributes.h"
#include "graphplugin.h"
#include "modelplugin.h"

namespace evoplex {

class ExperimentsMgr;
class Project;

typedef QSharedPointer<Project> ProjectSP;

class MainApp : public QObject
{
    Q_OBJECT

public:
    static const char* kPluginExtension;

    explicit MainApp();
    ~MainApp();

    void resetSettingsToDefault();

    // load plugin from a .so file; return nullptr if unsuccessful
    const AbstractPlugin* loadPlugin(const QString& path, QString& error, const bool addToUserPrefs);

    // unload an existing plugin; return true if successful
    bool unloadPlugin(const AbstractPlugin* plugin, QString& error);

    // Create a new project
    ProjectSP newProject(QString &error, const QString& filepath="");

    void closeProject(int projId);

    inline quint16 defaultStepDelay() const { return m_defaultStepDelay; }
    void setDefaultStepDelay(quint16 msec);

    inline int stepsToFlush() const { return m_stepsToFlush; }
    void setStepsToFlush(int steps);

    inline ExperimentsMgr* expMgr() const { return m_experimentsMgr; }
    inline const QHash<QString, GraphPlugin*>& graphs() const { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& models() const { return m_models; }
    inline const std::map<int, ProjectSP>& projects() const { return m_projects; }

    inline const GraphPlugin* graph(const QString& graphId) const { return m_graphs.value(graphId, nullptr); }
    inline const ModelPlugin* model(const QString& modelId) const { return m_models.value(modelId, nullptr); }
    inline ProjectSP project(int projId) const { return m_projects.at(projId); }

    inline const AttributesScope& generalAttrsScope() const { return m_generalAttrsScope; }

    void addPathToRecentProjects(const QString& projectFilePath);

signals:
    void pluginAdded(const AbstractPlugin* plugin);
    void pluginRemoved(const QString& id, AbstractPlugin::PluginType t);
    void listOfRecentProjectsUpdated();

private:
    // Load built-in plugins
    void initSystemPlugins();

    // Load user imported plugins.
    // It removes failed plugins from the list
    void initUserPlugins();

    ExperimentsMgr* m_experimentsMgr;
    QDir m_systemPluginsDir;

    QSettings m_userPrefs;
    quint16 m_defaultStepDelay; // msec
    int m_stepsToFlush;

    std::map<int, ProjectSP> m_projects; // opened projects.

    QHash<QString, GraphPlugin*> m_graphs;  // loaded graphs
    QHash<QString, ModelPlugin*> m_models;  // loaded models

    // let's build a hash with the name and attrRange of the essential attributes
    // it is important to validate the contents of csv files
    AttributesScope m_generalAttrsScope;
};
}

#endif // MAINAPP_H
