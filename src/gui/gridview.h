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

#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include "graphwidget.h"

namespace evoplex
{

class GridView : public GraphWidget
{
    Q_OBJECT

public:
    explicit GridView(MainGUI* mainGUI, ExperimentPtr exp, ExperimentWidget* parent);

protected:
    void paintEvent(QPaintEvent*) override;
    NodePtr selectNode(const QPoint& pos) const override;
    CacheStatus refreshCache() override;

private:
    struct Cache {
        NodePtr node = nullptr;
        QRectF rect;
    };
    std::vector<Cache> m_cache;
};

} // evoplex
#endif // GRIDVIEW_H
