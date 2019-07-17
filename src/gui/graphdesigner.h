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

#ifndef GRAPH_DESIGNER_H
#define GRAPH_DESIGNER_H

#include <QDockWidget>
#include <QMainWindow>

#include "core/project.h"

#include "graphwidget.h"

namespace evoplex {

class GraphDesigner : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphDesigner(MainApp* mainApp, QMainWindow *parent);
    ~GraphDesigner();

private:
    MainApp* m_mainApp;
    QMainWindow* m_innerWindow;

    int m_curGraphId;

    void initEmptyGraph();
};
}

#endif // GRAPH_DESIGNER_H