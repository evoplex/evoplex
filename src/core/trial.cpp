/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/trial.h"
#include "core/mainapp.h"
#include "core/project.h"

Trial::Trial(int id, int expId, int projId, AbstractModel* modelObj, int stopAt)
    : m_id(id)
    , m_expId(expId)
    , m_projId(projId)
    , m_modelObj(modelObj)
    , m_stopAt(stopAt)
    , m_pauseAt(stopAt)
    , m_currentStep(0)
    , m_status(READY)
{
}

Trial::~Trial()
{
    delete m_modelObj;
    m_modelObj = nullptr;
}

void Trial::processSteps()
{
    if (m_status != READY) {
        return;
    }

    m_status = RUNNING;
    emit (statusChanged(m_id, m_expId, m_projId, m_status));

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
    emit (statusChanged(m_id, m_expId, m_projId, m_status));
}

void Trial::finish()
{
    Q_ASSERT(m_status == FINISHING);

    // nothing to do yet
}
