/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TEST_H
#define TEST_H

#include "core/abstractmodel.h"

namespace evoplex {

class ModelTest: public AbstractModel
{
public:
    bool init() { return true; }
    bool algorithmStep();

private:
    enum ModelAttr { Temptation, Loners };
    enum AgentAttr { Strategy };
};
}

REGISTER_MODEL(ModelTest)

#endif // TEST_H
