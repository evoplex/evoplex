/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef POPULATION_GROWTH_H
#define POPULATION_GROWTH_H

#include <plugininterface.h>

namespace evoplex {
class PopulationGrowth: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;

private:
    int m_infectedAttrId;   // the id of the 'infected' node's attribute
    double m_prob;          // probability of a node becoming infected
};
} // evoplex
#endif // POPULATION_GROWTH_H
