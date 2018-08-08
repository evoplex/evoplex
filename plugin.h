/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#ifndef POPULATION_GROWTH_MODEL_H
#define POPULATION_GROWTH_MODEL_H

#include <plugininterface.h>

namespace evoplex {
class PopulationGrowth: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    int m_infectedAttrId;
    int m_prob;
};
} // evoplex
#endif // POPULATION_GROWTH_MODEL_H
