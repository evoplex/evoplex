#include <QtDebug>

#include "model.h"

ModelHOPD::ModelHOPD()
    : m_b(1.6)
    , m_l(0.3)
{
}

bool ModelHOPD::init(const QVariantHash& modelParams)
{
    return true;
}

bool ModelHOPD::algorithmStep()
{
    qDebug() << "HOPD";
    return false;
}
