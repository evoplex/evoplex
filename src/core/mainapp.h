/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QList>
#include <QMetaProperty>
#include <QObject>
#include <QVariantHash>

#include "core/interfaces.h"
#include "core/processesmgr.h"

// these constants hold the name of the properties used in any simulation
#define GENERAL_PROPERTY_NAME_AGENTS "agents"
#define GENERAL_PROPERTY_NAME_GRAPHTYPE "graphType"
#define GENERAL_PROPERTY_NAME_SEED "seed"
#define GENERAL_PROPERTY_NAME_STOPAT "stopAt"

class Project;
class Simulation;

class MainApp: public QObject
{
    Q_OBJECT

public:
    struct Model {
        QString author;
        QString name;
        QString description;
        QVariantHash agentParamsDomain;
        QVariantHash defaultModelParams;
        QVariantHash defaultInspectorParams;
        IModelFactory* factory;
    };

    MainApp();
    ~MainApp();

    // load model from a .so file
    // return the model id
    int loadModel(QString path);

    int newProject(int modelId, const QString& name,
            const QString& descr, const QString& dir);

    inline ProcessesMgr* getProcessesMgr() { return m_processesMgr; }
    inline const QHash<int, Model*>& getModels() { return m_models; }
    inline const QHash<int, Project*>& getProjects() { return m_projects; }
    inline Model* getModel(int modelId) { return m_models.value(modelId); }
    inline Project* getProject(int projId) { return m_projects.value(projId); }
    inline const QStringList& getGeneralProperties() { return m_generalProperties; }

private:
    ProcessesMgr* m_processesMgr;
    QHash<int, Model*> m_models;
    QHash<int, Project*> m_projects;

    // lets build a list with the name of the essential properties
    QStringList m_generalProperties;
};

#endif // MAINAPP_H
