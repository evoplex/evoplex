/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#ifndef MINIMAL_MODEL_H
#define MINIMAL_MODEL_H

#include <plugininterface.h>

namespace evoplex {
class MinimalModel: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    int m_infected;
    int m_infected_next_state;
    int m_prob;
    int m_number_of_nodes;
};
} // evoplex
#endif // MINIMAL_MODEL_H
