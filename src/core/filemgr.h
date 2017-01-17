/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef FILEMGR_H
#define FILEMGR_H

#include "core/abstractagent.h"
#include "core/mainapp.h"

class FileMgr
{
public:
    explicit FileMgr(MainApp* mainApp);
    virtual ~FileMgr() {}

    // Import a set of agents from a csv file
    // Agents belong to some model. That's why we need the modelId,
    // which is also important to validate the inputs in the file.
    QVector<AbstractAgent*> importAgents(const QString& filePath, const QString& modelId);

    // Import a set of experiments from a csv file
    // return the number of failures or -1 if everything went wrong.
    // Experiments belong to a project, that is why we need a valid projectId.
    int importExperiments(const QString& filePath, int projId);

private:
    MainApp* m_mainApp;
};

#endif // FILEMGR_H
