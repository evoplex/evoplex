/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef FILEMGR_H
#define FILEMGR_H

#include "agent.h"
#include "mainapp.h"

namespace evoplex {

class FileMgr
{
public:
    explicit FileMgr(MainApp* mainApp);

    // Import a set of agents from a csv file
    // Agents belong to some model. That's why we need the modelId,
    // which is also important to validate the inputs in the file.
    Agents importAgents(const QString& filePath, const QString& modelId) const;

private:
    MainApp* m_mainApp;
};
}
#endif // FILEMGR_H
