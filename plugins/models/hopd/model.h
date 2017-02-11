/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef HOPD_H
#define HOPD_H

#include <QObject>
#include <QVariantHash>

#include "core/abstractmodel.h"

class ModelHOPD: public AbstractModel
{
public:
    ModelHOPD();
    bool init(const QVariantHash& modelParams);
    bool algorithmStep();

private:
    double m_b;
    double m_l;
};

REGISTER_MODEL(ModelHOPD)

#endif // HOPD_H
