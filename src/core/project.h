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

namespace evoplex
{
class Project : public QObject
{
    Q_OBJECT

public:
    Project(MainApp* mainApp, int id, const QString& name="", const QString& dest="");
    virtual ~Project();

    // Add a new experiment to this project.
    // @return nullptr if unsuccessful
    Experiment* newExperiment(Experiment::ExperimentInputs* inputs);

    // Edit an experiment of this project.
    bool editExperiment(int expId, Experiment::ExperimentInputs* newInputs);

    // Import a set of experiments from a csv file
    // return the number of failures or -1 if everything went wrong.
    // Experiments belong to a project, that is why we need a valid projectId.
    const int importExperiments(const QString& filePath);

    // Save project into the dest directory.
    // A project is composed of plain csv files
    bool saveProject(QString& errMsg, std::function<void(int)>& progress);

    // execute all experiments of this project.
    void playAll();

    inline const QString& name() const { return m_name; }
    inline void setName(const QString& name) { m_name = name; }

    inline const QString& dest() const { return m_dest; }
    inline void setDest(const QString& dest) { m_dest = dest; }

    inline Experiment* experiment(int expId) const { return m_experiments.value(expId); }
    inline const QHash<int, Experiment*> experiments() const { return m_experiments; }

    inline int id() const { return m_id; }
    inline const QHash<QString, GraphPlugin*>& graphs() const { return m_mainApp->graphs(); }
    inline const QHash<QString, ModelPlugin*>& models() const { return m_mainApp->models(); }
    inline bool hasUnsavedChanges() const { return m_hasUnsavedChanges; }

signals:
    void expAdded(int expId);
    void expEdited(int expId);
    void hasUnsavedChanges(bool);

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
