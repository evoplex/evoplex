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

    QString loadModel(QString path);

    Project* newProject(const QString& modelName, const QString& name,
            const QString& descr, const QString& dir);

    inline ProcessesMgr* getProcessesMgr() { return m_processesMgr; }
    inline const QList<Model*> getModels() { return m_models.values(); }
    inline const Model* getModel(QString name) { return m_models.value(name); }

private:
    ProcessesMgr* m_processesMgr;
    QMap<QString, Model*> m_models;
    QList<Project*> m_openedProjects;

};

#endif // MAINAPP_H
