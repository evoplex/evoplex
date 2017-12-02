/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TEST_H
#define TEST_H

#include "core/plugininterfaces.h"

namespace evoplex {
class ModelTest: public AbstractModel
{
public:
    virtual bool init();
    virtual bool algorithmStep();

private:
    enum ModelAttr { Temptation, Loners };
    enum AgentAttr { Strategy };
};
}

#endif // TEST_H
