/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOWAK92_H
#define NOWAK92_H

#include <evoplex/plugininterfaces.h>

namespace evoplex {
class ModelNowak: public AbstractModel
{
public:
    virtual bool init();
    virtual bool algorithmStep();

private:
    enum AgentAttr { Strategy, Score };

    double m_temptation;

    const double playGame(const int sX, const int sY) const;
    const int binarize(const int strategy) const;
};
}

#endif
