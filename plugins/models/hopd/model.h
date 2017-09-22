/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef HOPD_H
#define HOPD_H

#include <QObject>
#include <QVariantHash>

#include "core/abstractmodel.h"

class ModelHOPD: public AbstractModel
{
public:
    ModelHOPD();
    bool init();
    bool algorithmStep();

    double playGame(AbstractAgent* agentX, AbstractAgent* agentY) const;

private:
    enum ModelAttr { B, L, MutRate };
    enum AgentAttr { STRATEGY, PROBTOA };

    double m_mutRate;
};

REGISTER_MODEL(ModelHOPD)

#endif // HOPD_H
