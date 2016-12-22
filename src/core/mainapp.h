/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QList>
#include <QVariantHash>

#include "core/abstractgraph.h"
#include "core/abstractmodel.h"
#include "core/processesmgr.h"

// these constants hold the name of the properties used in any simulation
#define GENERAL_PARAMETER_AGENTS "agents"
#define GENERAL_PARAMETER_GRAPHID "graphId"
#define GENERAL_PARAMETER_SEED "seed"
#define GENERAL_PARAMETER_STOPAT "stopAt"

class Project;
class Simulation;

class MainApp
{
public:
    struct GraphPlugin {
        QString uid;
        QString author;
        QString name;
        QString description;
        QVariantHash graphParamsSpace;
        IPluginGraph* factory;
    };

    struct ModelPlugin {
        QString uid;
        QString author;
        QString name;
        QString description;
        QVariantHash agentParamsSpace;
        QVariantHash modelParamsSpace;
        QVariantHash defaultAgentParams;
        QVariantHash defaultModelParams;
        IPluginModel* factory;
    };

    MainApp();
    ~MainApp();

    // load graph from a .so file
    // return the graph uid
    const QString &loadGraphPlugin(QString path);

    // load model from a .so file
    // return the model uid
    const QString &loadModelPlugin(QString path);

    int newProject(const QString &modelId, const QString& name,
            const QString& descr, const QString& dir);

    inline ProcessesMgr* getProcessesMgr() { return m_processesMgr; }
    inline const QHash<QString, GraphPlugin*>& getGraphs() { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& getModels() { return m_models; }
    inline const QHash<int, Project*>& getProjects() { return m_projects; }
    inline GraphPlugin* getGraph(const QString& graphId) { return m_graphs.value(graphId); }
    inline ModelPlugin* getModel(const QString& modelId) { return m_models.value(modelId); }
    inline Project* getProject(int projId) { return m_projects.value(projId); }
    inline const QStringList& getGeneralProperties() { return m_generalProperties; }

private:
    ProcessesMgr* m_processesMgr;

    QHash<QString, GraphPlugin*> m_graphs;
    QHash<QString, ModelPlugin*> m_models;
    QHash<int, Project*> m_projects;

    // lets build a list with the name of the essential properties
    QStringList m_generalProperties;
};

#endif // MAINAPP_H
