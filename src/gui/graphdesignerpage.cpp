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
#include "ui_graphdesignerpage.h"
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
    QAction* acEdgeAttrs = new QAction(QIcon(":/icons/settings.svg"), "EdgeAttributesEditor", actionGroup);
    acEdgeAttrs->setCheckable(true);
    toolbar->addAction(acEdgeAttrs);

    QAction* acNodeAttrs = new QAction(QIcon(":/icons/settings.svg"), "NodeAttributesEditor", actionGroup);
    acNodeAttrs->setCheckable(true);
    toolbar->addAction(acNodeAttrs);

    QAction* acGraphGen = new QAction(QIcon(":/icons/settings.svg"), "GraphGenerator", actionGroup);
    acGraphGen->setCheckable(true);
    toolbar->addAction(acGraphGen);

    toolbar->setIconSize(QSize(32, 32));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addToolBar(toolbar);
    for (QToolButton* btn : toolbar->findChildren<QToolButton*>()) {
        btn->installEventFilter(this);
    }

}

GraphDesignerPage::~GraphDesignerPage()
{
}

}
