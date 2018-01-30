/**
 * Copyright (C) 2016-2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "model.h"

namespace evoplex
{

bool ModelTemplate::init()
{
    return true;
}

bool ModelTemplate::algorithmStep()
{
    return false;
}

} // evoplex
REGISTER_MODEL(ModelTemplate)
#include "model.moc"
