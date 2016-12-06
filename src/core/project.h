/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QVariantHash>

#include "core/interfaces.h"
#include "core/graph.h"
#include "core/mainapp.h"
#include "core/simulation.h"

class Project: public QObject
{
    Q_OBJECT

public:
    Project(MainApp *mainApp, int projId, int modelId, const QString& name,
            const QString& descr="", const QString& dir="");

    virtual ~Project();

    QVector<IAgent*> getAgentsFromFile(const QString& path);

    // return processId
    int runExperiment(const int expId);

    Simulation* newExperiment(const QVariantHash& generalParams, const QVariantHash& modelParams);

    QList<Simulation*> importExperiments(const QString& filePath, QVariantHash &generalParams, QVariantHash &modelParams);

    QVariantHash getGeneralParams(int eId);
    QVariantHash getModelParams(int eId);

    inline Simulation* getExperiment(int eId) { return m_experiments.value(eId); }
    inline const QString& getName() { return m_name; }
    inline const QString& getDir() { return m_dir; }
    inline MainApp::Model* getModel() { return m_model; }

private:
    MainApp* m_mainApp;
    const int m_projId;
    const int m_modelId;
    MainApp::Model* m_model;
    QString m_name;
    QString m_description;
    QString m_dir;

    QHash<int, Simulation*> m_experiments;
};

#endif // PROJECT_H
