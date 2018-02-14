/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "prg.h"

namespace evoplex
{

PRG::PRG(int seed)
    : m_mteng(seed)
    , m_doubleZeroOne(0.,1.)
    , m_floatZeroOne(0.f,1.f)
{
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
