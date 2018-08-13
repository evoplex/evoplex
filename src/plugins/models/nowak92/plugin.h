/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef NOWAK92_H
#define NOWAK92_H

#include <plugininterface.h>

namespace evoplex {
class ModelNowak: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    enum NodeAttr { STRATEGY, SCORE };

    double m_temptation;

    double playGame(const int sX, const int sY) const;
    int binarize(const int strategy) const;
};
} // evoplex
#endif
