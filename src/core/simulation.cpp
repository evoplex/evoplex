/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/simulation.h"
#include "core/mainapp.h"
#include "core/project.h"

Simulation::Simulation(int expId, int projId, int modelId, IModel* modelObj, const QVariantHash& generalParams)
    : m_experimentId(expId)
    , m_projectId(projId)
    , m_modelId(modelId)
    , m_processId(-1)
    , m_modelObj(modelObj)
    , m_status(INVALID)
    , m_currentStep(0)
{
    bool ok;
    m_stopAt = generalParams.value(GENERAL_PROPERTY_NAME_STOPAT).toInt(&ok);
    m_pauseAt = m_stopAt;

    if (!ok || m_stopAt < 0 || m_stopAt > MAX_STEP) {
        QString msg = QString("[Simulation]: unable to load the simulation."
                              "stopAt=%1 is invalid!").arg(m_stopAt);
        qWarning() << msg;
        return;
    }

    m_status = READY;
}

Simulation::~Simulation()
{
    delete m_modelObj;
    m_modelObj = NULL;
}

void Simulation::processSteps()
{
    if (m_status != READY) {
        return;
    }

    m_status = RUNNING;
    emit (statusChanged(m_experimentId, m_processId, m_status));

    bool algorithmConverged = false;
    while (m_currentStep <= m_pauseAt && !algorithmConverged) {
        algorithmConverged = m_modelObj->algorithmStep();
        ++m_currentStep;
    }

    if (m_currentStep >= m_stopAt || algorithmConverged) {
        m_status = FINISHING;
        finish();
        m_status = FINISHED;
    } else {
        m_pauseAt = m_stopAt; // reset the pauseAt flag to maximum
        m_status = READY;
    }
    emit (statusChanged(m_experimentId, m_processId, m_status));
}

void Simulation::finish()
{
    Q_ASSERT(m_status == FINISHING);

    // nothing to do yet
}
