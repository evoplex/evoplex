/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QVariantMap>

class Simulation: public QObject
{
    Q_OBJECT

public:
    Simulation(const quint16& processId, const QVariantMap& params);
    virtual ~Simulation();

    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processSteps();

    void pause() { m_pauseAt = m_currentStep; }
    void pauseAt(quint64 step) { m_pauseAt = step; }

    void stop() { m_pauseAt = m_currentStep; m_finishAt = m_currentStep; }
    void stopAt(quint64 step) { m_finishAt = step; }

    quint64 getCurrentStep() { return m_currentStep; }

protected:
    const quint32 m_processId;

private:
    enum SimulationStatus {
        INVALID,    // model was not loaded correctly
        READY,      // ready for another step
        RUNNING,    // simulation is running
        FINISHING,  // saving stuff
        FINISHED    // completely finished
    };

    SimulationStatus m_status;
    quint64 m_currentStep;
    quint64 m_pauseAt;      // use 0 to infinite
    quint64 m_finishAt;     // use 0 to infinite

    // Finishes this simulation.
    // Any IO operation will be called from here.
    void finish();

    // This method must be implemented by the metaheuristic and
    // have to contain all the logic to perform a single step.
    virtual void algorithmStep() = 0;

};

#endif // SIMULATION_H
