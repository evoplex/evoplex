/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#include "prg.h"

namespace evoplex {

PRG::PRG(unsigned int seed)
    : m_seed(seed),
      m_mteng(seed),
      m_doubleZeroOne(0.0, 1.0),
      m_floatZeroOne(0.f, 1.f),
      m_bernoulli(0.5)
{
}

bool PRG::randBernoulli(double p)
{
    std::bernoulli_distribution b(p);
    return b(m_mteng);
}

double PRG::randD(double min, double max)
{
    std::uniform_real_distribution<double> dis(min, max);
    return dis(m_mteng);
}

float PRG::randF(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(m_mteng);
}

int PRG::randI(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(m_mteng);
}

size_t PRG::randS(size_t min, size_t max)
{
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(m_mteng);
}

} // evoplex
