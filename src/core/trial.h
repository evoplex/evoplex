/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TRIAL_H
#define TRIAL_H

#include <QObject>
#include <QVariantMap>

#include "core/abstractmodel.h"
#include "utils/constants.h"

// Here is where the actual simulation is performed. A trial is part of an experiment which might
// have several other trials. All trials of an experiment are meant to use exactly the same
// parameters and population of agents. Be aware that each trial will use the root seed incremented
// by 1. For exemple, if an experiment with 3 trials has the seed '111', the seeds of the trials
// will be '111', '112' and '113'.
class Trial: public QObject
{
    Q_OBJECT

public:
    enum Status {
        // ready for another step
        READY,
        // simulation is running in some work thread
        RUNNING,
        // simulation is over.
        // Now doing the post-experiment part which is usually an output operation
        FINISHING,
        // all is done
        FINISHED,
        // let the other classes aware that this simulation must be deleted
        KILLED
    };

    // getters
    inline int getId() { return m_id; }
    inline int getExpId() { return m_expId; }
    inline int getProjId() { return m_projId; }
    inline Status getStatus() { return m_status; }
    inline int getCurrentStep() { return m_currentStep; }
    inline int getPauseAt() { return m_pauseAt; }
    inline int getStopAt() { return m_stopAt; }

signals:
    void statusChanged(int trialId, int expId, int projId, int newStatus);

private:
    const int m_id;
    const int m_expId;
    const int m_projId;

    AbstractModel* m_modelObj;
    int m_stopAt;
    int m_pauseAt;
    int m_currentStep;
    Status m_status;

    // Finishes this simulation.
    // Any IO operation will be called from here.
    void finish();

    // Trials are meant to be created and managed by the TrialsMgr class.
    friend class TrialsMgr;
    explicit Trial(int id, int expId, int projId, AbstractModel* modelObj, int stopAt);
    virtual ~Trial();

    // This method will run in a worker thread until it reaches the max
    // number of steps or the pause criteria defined by the user.
    void processSteps();

    // All these methods to control the simulation should also only be accessed by the
    // TrialsMgr class. Otherwise, we might have issues with our concurrent system.
    inline void pause() { m_pauseAt = m_currentStep; }
    inline void pauseAt(int step) { m_pauseAt = step > EVOPLEX_MAX_STEPS ? EVOPLEX_MAX_STEPS : step; }
    inline void stop() { m_pauseAt = m_currentStep; m_stopAt = m_currentStep; }
    inline void stopAt(int step) { m_stopAt = step > EVOPLEX_MAX_STEPS ? EVOPLEX_MAX_STEPS : step; }
};

#endif // TRIAL_H
