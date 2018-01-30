/**
 * Copyright (C) 2016-2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef TEMPLATEMODEL_H
#define TEMPLATEMODEL_H

#include <core/plugininterfaces.h>

namespace evoplex {
class ModelNowak: public AbstractModel
{
public:
    virtual bool init();
    virtual bool algorithmStep();
};
}

#endif
