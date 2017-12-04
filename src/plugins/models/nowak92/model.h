/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef NOWAK92_H
#define NOWAK92_H

#include <core/plugininterfaces.h>

namespace evoplex {
class ModelNowak: public AbstractModel
{
public:
    virtual bool init();
    virtual bool algorithmStep();

private:
    enum AgentAttr { Strategy, Score };

    double m_temptation;

    double playGame(int sX, int sY);
};
}

#endif
