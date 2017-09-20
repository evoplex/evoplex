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

    double playGame(int sA, int sB) const;

private:
    enum ModelAttr { B, L };
    enum AgentAttr { STRATEGY, PROBTOA };
};

REGISTER_MODEL(ModelHOPD)

#endif // HOPD_H
