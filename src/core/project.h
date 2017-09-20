/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QVariantHash>
#include <QVector>

#include "core/abstractmodel.h"
#include "core/mainapp.h"
#include "core/experiment.h"

class Project
{
public:
    Project(MainApp* mainApp, int id, const QString& name="", const QString& dir="");
    virtual ~Project();

    // Add a new experiment to this project. We assume that all graph/model
    // attributes start with 'uid_'. It is very important to avoid clashes
    // between different attributes which use the same name.
    // @return the experimentId or -1 if unsuccessful
    const int newExperiment(const QStringList& header, const QStringList& values, QString& errorMsg);

    // Import a set of experiments from a csv file
    // return the number of failures or -1 if everything went wrong.
    // Experiments belong to a project, that is why we need a valid projectId.
    const int importExperiments(const QString& filePath);

    // execute all experiments of this project.
    void runAll();

    inline int getId() { return m_id; }
    inline const QString& getName() { return m_name; }
    inline const QString& getDir() { return m_dir; }
    inline Experiment* getExperiment(int expId) { return m_experiments.value(expId); }

    inline const QHash<QString, MainApp::GraphPlugin*>& getGraphs() { return m_mainApp->getGraphs(); }
    inline const QHash<QString, MainApp::ModelPlugin*>& getModels() { return m_mainApp->getModels(); }

private:
    MainApp* m_mainApp;
    const int m_id;
    QString m_name;
    QString m_dir;

    int m_lastExpId;
    QHash<int, Experiment*> m_experiments;
};

#endif // PROJECT_H
