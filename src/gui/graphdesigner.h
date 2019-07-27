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

#include "core/graphinputs.h"
#include "core/project.h"

#include "abstractgraph.h"
#include "graphwidget.h"
#include "graphdesignerpage.h"
#include "maingui.h"

namespace evoplex {

class GraphDesignerPage;

class BaseAbstractGraph : public AbstractGraph
{

public:
    bool reset() override { return 0; };

};

class GraphDesigner : public QDockWidget
{
    Q_OBJECT
   friend class GraphDesignerPage;

public:
    explicit GraphDesigner(MainGUI* mainGUI, GraphDesignerPage *parent);
    ~GraphDesigner();

public slots:
    void slotUpdateGraph(QString& error);
    void slotUpdateAttrs();
    void slotOpenSettings();
    void slotExportNodes();

private:
    MainGUI * m_mainGUI;
    MainApp* m_mainApp;
    QMainWindow* m_innerWindow;
    GraphDesignerPage* m_parent;
    GraphWidget* m_curGraph;
    int m_curGraphId;
    AbstractGraph* m_abstrGraph;

    void initEmptyGraph();
    bool readyToExport();
    GraphInputsPtr parseInputs(QString& error);
};


}

#endif // GRAPH_DESIGNER_H