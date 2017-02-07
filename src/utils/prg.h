/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PRG_H
#define PRG_H

#include <random>

class PRG
{
public:
    explicit PRG(int seed):m_mteng(seed), m_zeroOne(0, 1) {}

    // Generate a random double [0, 1)
    double randD() { return m_zeroOne(m_mteng); }

    // Generate a random double [min, max)
    double randD(double min, double max) {
        std::uniform_real_distribution<double> dis(min, max);
        return dis(m_mteng);
    }
    // Generate a random double [0, max)
    double randD(double max) { return randD(0.0, max); }

    // Generate a random int [min, max)
    int randI(int min, int max) {
        std::uniform_int_distribution<int> dis(min, max);
        return dis(m_mteng);
    }
    // Generate a random int [0, max)
    int randI(int max) { return randI(0, max); }

private:
    std::mt19937 m_mteng; //  Mersenne Twister engine
    std::uniform_real_distribution<double> m_zeroOne;
};

#endif // PRG_H
