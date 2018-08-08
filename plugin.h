/**
 * Evoplex <https://evoplex.github.com>
 * Copyright (C) 2016-present
 */

#ifndef GAME_OF_LIFE_MODEL_H
#define GAME_OF_LIFE_MODEL_H

#include <plugininterface.h>

namespace evoplex {
class GameOfLifeModel: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    bool m_live;
};
} // evoplex
#endif // GAME_OF_LIFE_MODEL_H
