/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QString>
#include <QVariantHash>
#include <QVector>

#include "core/mainapp.h"

// Evoplex assumes that any experiment belong to a project.
// Cosidering that a project might have a massive amount of experiments, this class
// should be as light as possible and should not do any parameter validation. So,
// we assume that everything is valid at this point.
class Experiment
{
public:
    explicit Experiment(MainApp* mainApp, int id, int projId, const QVariantHash& generalParams,
        const QVariantHash& modelParams, const QVariantHash& graphParams);

    // destructor
    virtual ~Experiment() {}

    // This method will create everything that is necessary to run the trials.
    // The only reason why we do not create them in the constructor is because
    // this process is very expensive and the user may not want to run the
    // trials anyway, so, we would be just wasting time.
    //
    // We can safely consider that all parameters are valid at this point.
    // However, some things might fail (eg, missing agents, broken graph etc),
    // and, in that case, we return false to indicate that something went wrong.
    bool createTrials();

    // getters
    inline int getId() { return m_id; }
    inline int getProjId() { return m_projId; }

private:
    MainApp* m_mainApp;
    const int m_id;
    const int m_projId;

    const QVariantHash& m_generalParams;
    const QVariantHash& m_modelParams;
    const QVariantHash& m_graphParams;

    QVector<int> m_trialsIds;
};

#endif // EXPERIMENT_H
