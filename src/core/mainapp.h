/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QHash>
#include <QVector>

#include "plugininterfaces.h"
#include "attributes.h"
#include "graphplugin.h"
#include "modelplugin.h"

namespace evoplex {

class ExperimentsMgr;
class FileMgr;
class Project;

class MainApp
{
public:
    MainApp();
    ~MainApp();

    // load plugin from a .so file; return true if successful
    const QString loadPlugin(const QString& path, QString& error);

    // Create a new project
    Project* newProject(const QString& name="", const QString& dest="");

    Project* openProject(const QString& filepath);

    void closeProject(int projId);

    inline FileMgr* getFileMgr() const { return m_fileMgr; }
    inline ExperimentsMgr* getExperimentsMgr() const { return m_experimentsMgr; }
    inline const QHash<QString, GraphPlugin*>& getGraphs() const { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& getModels() const { return m_models; }
    inline const QHash<int, Project*>& getProjects() const { return m_projects; }

    inline const GraphPlugin* getGraph(const QString& graphId) const { return m_graphs.value(graphId, nullptr); }
    inline const ModelPlugin* getModel(const QString& modelId) const { return m_models.value(modelId, nullptr); }
    inline Project* getProject(int projId) const { return m_projects.value(projId); }

    inline const AttributesSpace& getGeneralAttrSpace() const { return m_generalAttrSpace; }

private:
    FileMgr* m_fileMgr;
    ExperimentsMgr* m_experimentsMgr;

    int m_lastProjectId;
    QHash<int, Project*> m_projects; // opened projects.

    QHash<QString, GraphPlugin*> m_graphs;  // loaded graphs
    QHash<QString, ModelPlugin*> m_models;  // loaded models

    // lets build a hash with the name and space of the essential parameters
    // it is important to validate the contents of csv files
    AttributesSpace m_generalAttrSpace;
};
}

#endif // MAINAPP_H
