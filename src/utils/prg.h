/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PRG_H
#define PRG_H

#include "QVariant"

class PRG
{
public:
    explicit PRG(int seed);

    // Generate a random double [0, 1)
    double randD();
    // Generate a random double [min, max)
    double randD(QVariant min, QVariant max);

    // Generate a random integer [0, max)
    int randI(int max);
    // Generate a random integer [min, max)
    int randI(int min, int max);

private:
    const int m_prg;
};

#endif // PRG_H
