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

#ifndef PRG_H
#define PRG_H

#include <random>

namespace evoplex {

/**
 * @brief Pseudo-random number generator.
 *        Based on the classic Mersenne Twister (std::mt19937)
 */
class PRG
{
public:
    explicit PRG(unsigned int seed);

    // Returns the PRG seed
    inline unsigned int seed() const
    { return m_seed; }

    // Generate a random boolean according to the discrete probability function
    // Where the probability of true is p and the probability of false is (1-p)
    inline bool bernoulli(double p)
    { std::bernoulli_distribution b(p); return b(m_mteng); }

    // randBernoulli(p=0.5)
    inline bool bernoulli()
    { return m_bernoulli(m_mteng); }

    // Generate a random double/float [min, max)
    template <typename T>
    T uniform(T min, T max)
    { std::uniform_real_distribution<T> d(min, max); return d(m_mteng); }

    // Generate a random integer [min, max]
    inline int uniform(int min, int max)
    { std::uniform_int_distribution<int> d(min, max); return d(m_mteng); }

    // Generate a random size_t [min, max]
    inline size_t uniform(size_t min, size_t max)
    { std::uniform_int_distribution<size_t> d(min, max); return d(m_mteng); }

    // Generate a random double/float [0, max)
    template <typename T>
    T uniform(T max)
    { std::uniform_real_distribution<T> d(0, max); return d(m_mteng); }

    // Generate a random integer [0, max]
    inline int uniform(int max)
    { std::uniform_int_distribution<int> d(0, max); return d(m_mteng); }

    // Generate a random size_t [0, max]
    inline size_t uniform(size_t max)
    { std::uniform_int_distribution<size_t> d(0, max); return d(m_mteng); }

    // Generate a random double [0, 1)
    inline double uniform()
    { return m_doubleZeroOne(m_mteng); }

    template <typename T>
    inline T uniform(std::uniform_real_distribution<T> d)
    { return d(m_mteng); }

    template <typename T>
    T uniform(std::uniform_int_distribution<T> d)
    { return d(m_mteng); }

private:
    const unsigned int m_seed;
    std::mt19937 m_mteng; //  Mersenne Twister engine
    std::uniform_real_distribution<double> m_doubleZeroOne;
    std::bernoulli_distribution m_bernoulli;
};

} // evoplex
#endif // PRG_H
