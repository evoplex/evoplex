/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TEST2_H
#define TEST2_H

#include "core/abstractmodel.h"

namespace evoplex {

class ModelTest2: public AbstractModel
{
public:
    bool init() { return true; }
    bool algorithmStep();

private:
    enum ModelAttr { Temptation, Loners };
    enum AgentAttr { Strategy };
};
}

REGISTER_MODEL(ModelTest2)

#endif // TEST2_H
