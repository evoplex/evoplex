/**
 * Copyright (C) 2016-2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "model.h"

namespace evoplex
{

bool TemplateModel::init()
{
    return true;
}

bool TemplateModel::algorithmStep()
{
    return false;
}

} // evoplex
REGISTER_MODEL(TemplateModel)
#include "model.moc"
