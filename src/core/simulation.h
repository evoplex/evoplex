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

    Simulation(int eId, IModel* model, const QVariantHash& generalParams);
    virtual ~Simulation();

    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processSteps();

    inline void pause() { m_pauseAt = m_currentStep; }
    inline void pauseAt(quint64 step) { m_pauseAt = step > MAX_STEP ? MAX_STEP : step; }

    inline void stop() { m_pauseAt = m_currentStep; m_stopAt = m_currentStep; }
    inline void stopAt(quint64 step) { m_stopAt = step > MAX_STEP ? MAX_STEP : step; }

    inline bool isValid() { return m_status != INVALID; }
    inline quint64 getCurrentStep() { return m_currentStep; }

    inline const Status& getStatus() { return m_status; }
    inline const int& getExperimentId() { return m_experimentId; }
    inline const int& getProcessId() { return m_processId; }
    inline void setProcessId(int processId) { m_processId = processId; }

signals:
    void statusChanged(int experimentId, int processId, int newStatus);

private:
    IModel* m_model;
    Status m_status;
    quint64 m_currentStep;
    quint64 m_pauseAt;
    quint64 m_stopAt;

    const int m_experimentId;
    int m_processId;

    // Finishes this simulation.
    // Any IO operation will be called from here.
    void finish();
};

#endif // SIMULATION_H
