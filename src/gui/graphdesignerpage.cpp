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

#include <QDebug>
#include <QToolBar>
#include <QToolButton>
#include <QHBoxLayout>

#include "fontstyles.h"
#include "graphdesignerpage.h"
#include "ui_graphdesignerpage.h"

namespace evoplex {

GraphDesignerPage::GraphDesignerPage(MainGUI* mainGUI)
    : QMainWindow(mainGUI),
    m_mainApp(mainGUI->mainApp()),
    m_mainGUI(mainGUI),
    m_innerWindow(new QMainWindow()),
    m_ui(new Ui_GraphDesignerPage),
    m_graphDesigner(new GraphDesigner(mainGUI->mainApp(), this))
{
    setWindowTitle("Graph Designer Page");
    setObjectName("GraphDesignerPage");
    m_ui->setupUi(this);

    
    setCentralWidget(m_graphDesigner);
}

GraphDesignerPage::~GraphDesignerPage()
{
}

}
