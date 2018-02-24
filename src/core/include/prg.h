/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PRG_H
#define PRG_H

#include <random>

namespace evoplex
{
class PRG
{
public:
    explicit PRG(int seed);

    // Generate a random double [0, 1)
    inline double randD() { return m_doubleZeroOne(m_mteng); }
    // Generate a random double [0, max)
    inline double randD(double max) { return randD(0.0, max); }
    // Generate a random double [min, max)
    double randD(double min, double max);

    // Generate a random float [0, 1)
    inline float randF() { return m_floatZeroOne(m_mteng); }
    // Generate a random float [0, max)
    inline float randF(float max) { return randF(0.f, max); }
    // Generate a random float [min, max)
    float randF(float min, float max);

    // Generate a random integer [0, max]
    inline int randI(int max) { return randI(0, max); }
    // Generate a random integer [min, max]
    int randI(int min, int max);

    // Generate a random integer [0, max]
    inline size_t randI(size_t max) { return randS(0, max); }
    // Generate a random integer [min, max]
    size_t randS(size_t min, size_t max);

private:
    std::mt19937 m_mteng; //  Mersenne Twister engine
    std::uniform_real_distribution<double> m_doubleZeroOne;
    std::uniform_real_distribution<float> m_floatZeroOne;
};
} // evoplex
#endif // PRG_H
