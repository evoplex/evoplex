#include "core/simulation.h"

Simulation::Simulation(const quint16& processId, const QVariantMap& params)
    : m_processId(processId)
    , m_status(INVALID)
    , m_currentStep(0)
    , m_pauseAt(0)
    , m_finishAt(0)
{
}

Simulation::~Simulation()
{
}


void Simulation::processSteps()
{
    if (m_status != READY) {
        return;
    }

    m_status = RUNNING;

    while (m_currentStep <= m_pauseAt) {
        algorithmStep();
        ++m_currentStep;
    }

    if (m_currentStep >= m_finishAt) {
        m_status = FINISHING;
        finish();
        m_status = FINISHED;
    } else {
        m_status = READY;
    }
}

void Simulation::finish()
{
    Q_ASSERT(m_status == FINISHING);

    // nothing to do yet
}
