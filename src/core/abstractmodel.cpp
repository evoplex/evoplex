/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#include "abstractmodel.h"
#include "trial.h"

namespace evoplex {

AbstractGraph* AbstractModel::graph() const
{
    return m_trial->graph();
}

int AbstractModel::currStep() const
{
    return m_trial->step();
}

int AbstractModel::status() const
{
    return m_trial->status();
}

inline Values AbstractModel::customOutputs(const Values& inputs) const
{
    Q_UNUSED(inputs);
    return Values();
}

} // evoplex
