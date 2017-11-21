/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QHash>
#include <QVector>

#include "abstractgraph.h"
#include "abstractmodel.h"
#include "attributes.h"
#include "enums.h"

namespace evoplex {

class ExperimentsMgr;
class FileMgr;
class Project;

class MainApp
{
public:

    struct GraphPlugin {
        QString uid;
        QString author;
        QString name;
        QString description;
        AttributesSpace graphAttrSpace;
        Attributes graphAttrMin;
        Attributes graphAttrMax;
        IPluginGraph* factory;
    };

    struct ModelPlugin {
        QString uid;
        QString author;
        QString name;
        QString description;
        QVector<QString> supportedGraphs;
        QVector<QString> customOutputs;
        AttributesSpace agentAttrSpace;
        AttributesSpace edgeAttrSpace;
        AttributesSpace modelAttrSpace;
        Attributes agentAttrMin;
        Attributes agentAttrMax;
        Attributes edgeAttrMin;
        Attributes edgeAttrMax;
        Attributes modelAttrMin;
        Attributes modelAttrMax;
        IPluginModel* factory;
    };

    MainApp();
    ~MainApp();

    // Create a new project
    Project* newProject(const QString& name="", const QString& dest="");

    Project* openProject(const QString& filepath);

    void closeProject(int projId);

    inline FileMgr* getFileMgr() { return m_fileMgr; }
    inline ExperimentsMgr* getExperimentsMgr() { return m_experimentsMgr; }
    inline const QHash<QString, GraphPlugin*>& getGraphs() { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& getModels() { return m_models; }
    inline const QHash<int, Project*>& getProjects() { return m_projects; }

    inline const GraphPlugin* getGraph(const QString& graphId) { return m_graphs.value(graphId, nullptr); }
    inline const ModelPlugin* getModel(const QString& modelId) { return m_models.value(modelId, nullptr); }
    inline Project* getProject(int projId) { return m_projects.value(projId); }

    inline const AttributesSpace& getGeneralAttrSpace() { return m_generalAttrSpace; }

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

    // load plugin from a .so file; return true if successful
    const QString loadPlugin(const QString& path);

    // load graph from a .so file
    // return the graph uid
    bool loadGraphPlugin(QObject* instance, QJsonObject& metaData);

    // load model from a .so file
    // return the model uid
    bool loadModelPlugin(QObject* instance, QJsonObject& metaData);

    AttributesSpace attributesSpace(const QJsonObject &metaData, const QString& name) const;
};
}

#endif // MAINAPP_H
