/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QList>
#include <QMetaProperty>
#include <QObject>

#include "core/interfaces.h"
#include "core/processes.h"

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
        QVariantMap params;
        IModelFactory* factory;
    };

    MainApp();
    ~MainApp();

    QString loadModel(QString path);

    Project* newProject(const QString& modelName, const QString& name,
            const QString& descr, const QString& dir);

    inline const Processes* getProcesses() { return m_processes; }
    inline const QList<Model*> getModels() { return m_models.values(); }
    inline const Model* getModel(QString name) { return m_models.value(name); }

private:
    Processes* m_processes;
    QMap<QString, Model*> m_models;
    QList<Project*> m_openedProjects;

};

#endif // MAINAPP_H
