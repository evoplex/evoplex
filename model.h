/**
 * Copyright (C) 2016-2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MODELTEMPLATE_H
#define MODELTEMPLATE_H

#include <core/plugininterfaces.h>

namespace evoplex {
class ModelTemplate: public AbstractModel
{
public:
    virtual bool init();
    virtual bool algorithmStep();
};
}

#endif
