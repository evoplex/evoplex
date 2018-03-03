/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QHash>
#include <QObject>
#include <QSettings>
#include <memory>
#include <QVector>

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

    inline const AttributesSpace& generalAttrSpace() const { return m_generalAttrSpace; }

    void addPathToRecentProjects(const QString& projectFilePath);

signals:
    void pluginAdded(const AbstractPlugin* plugin);
    void pluginRemoved(const QString& id, AbstractPlugin::PluginType t);
    void listOfRecentProjectsUpdated();

private:
    ExperimentsMgr* m_experimentsMgr;

    QSettings m_userPrefs;
    quint16 m_defaultStepDelay; // msec
    int m_stepsToFlush;

    std::map<int, ProjectSP> m_projects; // opened projects.

    QHash<QString, GraphPlugin*> m_graphs;  // loaded graphs
    QHash<QString, ModelPlugin*> m_models;  // loaded models

    // lets build a hash with the name and space of the essential parameters
    // it is important to validate the contents of csv files
    AttributesSpace m_generalAttrSpace;
};
}

#endif // MAINAPP_H
