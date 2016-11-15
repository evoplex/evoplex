/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>

#include "core/mainapp.h"
#include "core/simulation.h"

Simulation::Simulation(int eId, IModel *model, const QVariantHash& generalParams)
    : m_model(model)
    , m_status(INVALID)
    , m_currentStep(0)
    , m_experimentId(eId)
    , m_processId(-1)
{
    m_stopAt = generalParams.value(GENERAL_PROPERTY_NAME_STOPAT).toInt();
    m_pauseAt = m_stopAt;

    if (m_stopAt < 0 || m_stopAt > MAX_STEP) {
        QString msg = QString("[Simulation] unable to load the simulation."
                              "stopAt=%1 is invalid!").arg(m_stopAt);
        qWarning() << msg;
        return;
    }

    m_status = READY;
}

Simulation::~Simulation()
{
    delete m_model;
    m_model = NULL;
}


void Simulation::processSteps()
{
    if (m_status != READY) {
        return;
    }

    m_status = RUNNING;
    emit (statusChanged(m_experimentId, m_processId, m_status));

    while (m_currentStep <= m_pauseAt && m_model->algorithmStep()) {
        ++m_currentStep;
    }

    if (m_currentStep >= m_stopAt) {
        m_status = FINISHING;
        finish();
        m_status = FINISHED;
    } else {
        m_status = READY;
    }
    emit (statusChanged(m_experimentId, m_processId, m_status));
}

void Simulation::finish()
{
    Q_ASSERT(m_status == FINISHING);

    // nothing to do yet
}
