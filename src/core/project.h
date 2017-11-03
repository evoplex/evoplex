/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QObject>

#include "abstractmodel.h"
#include "mainapp.h"
#include "experiment.h"

namespace evoplex {

class Project : public QObject
{
    Q_OBJECT

public:
    Project(MainApp* mainApp, int id, const QString& name="", const QString& dest="");
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

    // Save project into the dest directory.
    // A project is composed by plain csv files
    bool saveProject(const QString &dest="", const QString &projectName="");

    // execute all experiments of this project.
    void playAll();

    inline const QString& getName() const { return m_name; }
    inline void setName(const QString& name) { m_name = name; }

    inline const QString& getDest() const { return m_dest; }
    inline void setDest(const QString& dest) { m_dest = dest; }

    inline int getId() const { return m_id; }
    inline Experiment* getExperiment(int expId) { return m_experiments.value(expId); }
    inline const QHash<QString, MainApp::GraphPlugin*>& getGraphs() const { return m_mainApp->getGraphs(); }
    inline const QHash<QString, MainApp::ModelPlugin*>& getModels() const { return m_mainApp->getModels(); }
    inline bool hasUnsavedChanges() const { return m_hasUnsavedChanges; }

signals:
    void hasUnsavedChanges(bool);
    // emit an integer [0,100] while saving this project
    void progressSave(int);

private:
    MainApp* m_mainApp;
    const int m_id;
    QString m_name;
    QString m_dest;
    bool m_hasUnsavedChanges;

    int m_lastExpId;
    QHash<int, Experiment*> m_experiments;
};
}

#endif // PROJECT_H
