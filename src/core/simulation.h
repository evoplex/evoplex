/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QVariantMap>

#include "core/interfaces.h"

#define MAX_STEP 100000000

class Simulation: public QObject
{
    Q_OBJECT

public:
    enum Status {
        INVALID,    // model was not loaded correctly
        READY,      // ready for another step
        RUNNING,    // simulation is running
        FINISHING,  // saving stuff
        FINISHED    // completely finished
    };

    Simulation(int expId, int projId, int modelId, IModel* modelObj, const QVariantHash& generalParams);
    virtual ~Simulation();

    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processSteps();

    inline void pause() { m_pauseAt = m_currentStep; }
    inline void pauseAt(int step) { m_pauseAt = step > MAX_STEP ? MAX_STEP : step; }

    inline void stop() { m_pauseAt = m_currentStep; m_stopAt = m_currentStep; }
    inline void stopAt(int step) { m_stopAt = step > MAX_STEP ? MAX_STEP : step; }

    inline bool isValid() { return m_status != INVALID; }
    inline int getCurrentStep() { return m_currentStep; }
    inline Status getStatus() { return m_status; }
    inline int getPauseAt() { return m_pauseAt; }
    inline int getStopAt() { return m_stopAt; }

    inline int getExperimentId() { return m_experimentId; }
    inline int getProjectId() { return m_projectId; }
    inline int getModelId() { return m_modelId; }

    inline int getProcessId() { return m_processId; }
    inline void setProcessId(int processId) { m_processId = processId; }

signals:
    void statusChanged(int experimentId, int processId, int newStatus);

private:
    const int m_experimentId;
    const int m_projectId;
    const int m_modelId;
    int m_processId;

    IModel* m_modelObj;
    Status m_status;
    int m_currentStep;
    int m_pauseAt;
    int m_stopAt;

    // Finishes this simulation.
    // Any IO operation will be called from here.
    void finish();
};

#endif // SIMULATION_H
