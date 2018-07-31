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
    bool m_live;
    bool m_live_next_state;
};
} // evoplex
#endif // MINIMAL_MODEL_H
