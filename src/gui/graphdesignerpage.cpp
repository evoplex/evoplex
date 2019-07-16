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

namespace evoplex {

GraphDesignerPage::GraphDesignerPage(MainGUI* mainGUI)
    : QMainWindow(mainGUI),
    m_mainApp(mainGUI->mainApp()),
    m_mainGUI(mainGUI),
    m_innerWindow(new QMainWindow()),
    m_graphDesigner(new GraphDesigner(mainGUI->mainApp(), this))
{
    setObjectName("GraphDesignerPage");
    setWindowTitle("GraphDesigner");
    setWindowIconText("GraphDesigner");
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);

    setDockNestingEnabled(false);
    setAnimated(true);

    QHBoxLayout* centralLayout = new QHBoxLayout(new QWidget(this));
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setCentralWidget(m_graphDesigner);

    // top toolbar
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setObjectName("topToolbar");
    toolbar->setStyleSheet("QToolButton{ margin: 0px 10px; }");

    QActionGroup* actionGroup = new QActionGroup(toolbar);
    QAction *acImgExport = new QAction(QIcon(":/icons/material/screenshot_white_18"), "Image Exporter", actionGroup);
    toolbar->addAction(acImgExport);

    QAction *acNodeExport = new QAction(QIcon(":/icons/material/table_white_18"), "Nodes Exporter", actionGroup);
    toolbar->addAction(acNodeExport);

    QAction *acGraphSettings = new QAction(QIcon(":/icons/material/settings_white_18"), "Graph Settings", actionGroup);
    toolbar->addAction(acGraphSettings);

    QAction* acEdgeAttrs = new QAction(QIcon(":/icons/settings.svg"), "Edge Attributes Editor", actionGroup);
    toolbar->addAction(acEdgeAttrs);

    QAction* acNodeAttrs = new QAction(QIcon(":/icons/settings.svg"), "Node Attributes Editor", actionGroup);
    toolbar->addAction(acNodeAttrs);

    QAction* acGraphGen = new QAction(QIcon(":/icons/settings.svg"), "Graph Generator", actionGroup);
    toolbar->addAction(acGraphGen);

    toolbar->setIconSize(QSize(32, 32));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addToolBar(toolbar);

}

GraphDesignerPage::~GraphDesignerPage()
{
}

}
