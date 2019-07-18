/**
*  This file is part of Evoplex.
*
*  Evoplex is a multi-agent system for networks.
*  Copyright (C) 2019 - Eleftheria Chatziargyriou
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

#ifndef GRAPHDESIGNERPAGE_H
#define GRAPHDESIGNERPAGE_H

#include <QMainWindow>

#include "maingui.h"
#include "abstractgraph.h"
#include "graphdesigner.h"

#include "core/mainapp.h"

class MainGUI;
class Ui_GraphDesignerPage;

namespace evoplex {

class GraphDesigner;

class GraphDesignerPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphDesignerPage(MainGUI* mainGUI);
    ~GraphDesignerPage();

private:
    Ui_GraphDesignerPage* m_ui;
    MainApp* m_mainApp;
    MainGUI* m_mainGUI;
    QMainWindow* m_innerWindow;
    GraphDesigner* m_graphDesigner;

private slots:
    void slotEdgeAttrs();
    void slotNodeAttrs();
    void slotGraphGen();
};

}

#endif // GRAPHDESIGNERPAGE_H
