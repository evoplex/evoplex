/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QHash>
#include <QVariantHash>
#include <QVector>

#include "core/abstractgraph.h"
#include "core/abstractmodel.h"
#include "core/trialsmgr.h"

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
        QHash<QString, QString> graphAttrSpace;
        QVariantHash graphAttrMin;
        QVariantHash graphAttrMax;
        IPluginGraph* factory;
    };

    struct ModelPlugin {
        QString uid;
        QString author;
        QString name;
        QString description;
        QVector<QString> supportedGraphs;
        QHash<QString, QString> agentAttrSpace;
        QHash<QString, QString> modelAttrSpace;
        QVariantHash agentAttrMin;
        QVariantHash agentAttrMax;
        QVariantHash modelAttrMin;
        QVariantHash modelAttrMax;
        IPluginModel* factory;
    };

    MainApp();
    ~MainApp();

    // load graph from a .so file
    // return the graph uid
    const QString& loadGraphPlugin(const QString& path);

    // load model from a .so file
    // return the model uid
    const QString& loadModelPlugin(const QString& path);

    // Create a new project; return its id
    int newProject(const QString& name="", const QString& dir="");

    inline FileMgr* getFileMgr() { return m_fileMgr; }
    inline TrialsMgr* getTrialsMgr() { return m_trialsMgr; }
    inline const QHash<QString, GraphPlugin*>& getGraphs() { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& getModels() { return m_models; }
    inline const QHash<int, Project*>& getProjects() { return m_projects; }

    inline const GraphPlugin* getGraph(const QString& graphId) { return m_graphs.value(graphId, nullptr); }
    inline const ModelPlugin* getModel(const QString& modelId) { return m_models.value(modelId, nullptr); }
    inline Project* getProject(int projId) { return m_projects.value(projId); }

    inline const QHash<QString, QString>& getGeneralAttrSpace() { return m_generalAttrSpace; }

private:
    FileMgr* m_fileMgr;
    TrialsMgr* m_trialsMgr;

    int m_lastProjectId;
    QHash<int, Project*> m_projects; // opened projects.

    QHash<QString, GraphPlugin*> m_graphs;  // loaded graphs
    QHash<QString, ModelPlugin*> m_models;  // loaded models

    // lets build a hash with the name and space of the essential parameters
    // it is important to validate the contents of csv files
    QHash<QString, QString> m_generalAttrSpace;

    // load plugin from a .so file; return true if successful
    bool loadPlugin(const QString& path, QObject* instance, QJsonObject& metaData);
};

#endif // MAINAPP_H
