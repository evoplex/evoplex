#include "model.h"

namespace evoplex {

bool ModelTest::init()
{
    return true;
}

bool ModelTest::algorithmStep()
{
    return false;
}

}

REGISTER_MODEL(ModelTest)
#include "model.moc"
