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

    // add a new experiment to this project
    // return true if successful
    bool newExperiment(const QVariantHash& generalParams,
            const QVariantHash& modelParams, const QVariantHash& graphParams);

    // add a new experiment to this project
    // return true if successful
    bool newExperiment(const QStringList& header, const QStringList& values);

    // getters
    inline int getId() { return m_id; }
    inline const QString& getName() { return m_name; }
    inline const QString& getDir() { return m_dir; }
    inline Experiment* getExperiment(int expId) { return m_experiments.value(expId); }

private:
    MainApp* m_mainApp;
    const int m_id;
    QString m_name;
    QString m_dir;

    int m_lastExpId;
    QHash<int, Experiment*> m_experiments;
};

#endif // PROJECT_H
