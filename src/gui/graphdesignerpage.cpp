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
#include <QSettings>

#include "fontstyles.h"
#include "graphdesignerpage.h"
#include "graphgendlg.h"
#include "ui_graphdesignerpage.h"

namespace evoplex {

GraphDesignerPage::GraphDesignerPage(MainGUI* mainGUI)
    : QMainWindow(mainGUI),
    m_mainApp(mainGUI->mainApp()),
    m_mainGUI(mainGUI),
    m_innerWindow(new QMainWindow()),
    m_ui(new Ui_GraphDesignerPage),
    m_graphDesigner(new GraphDesigner(mainGUI, this))
{
    setWindowTitle("Graph Designer Page");
    setObjectName("GraphDesignerPage");
    m_ui->setupUi(this);

    connect(m_ui->acEdgeAttrs, SIGNAL(triggered()), SLOT(slotEdgeAttrs()));
    connect(m_ui->acNodeAttrs, SIGNAL(triggered()), SLOT(slotNodeAttrs()));
    connect(m_ui->acGraphGen, SIGNAL(triggered()), SLOT(slotGraphGen()));
    connect(m_ui->acGraphSettings, SIGNAL(triggered()), m_graphDesigner, SLOT(slotOpenSettings()));
    connect(m_ui->acNodesExporter, SIGNAL(triggered()), m_graphDesigner, SLOT(slotExportNodes()));

    setCentralWidget(m_graphDesigner);
}

GraphDesignerPage::~GraphDesignerPage()
{
    delete m_ui;
}

void GraphDesignerPage::slotEdgeAttrs() {
    new GraphAttrsDlg(this, AttrsType::Edges);
}

void GraphDesignerPage::slotNodeAttrs()
{
    new GraphAttrsDlg(this, AttrsType::Nodes);
}

void GraphDesignerPage::slotGraphGen()
{
    new GraphGenDlg(this, m_mainGUI);
}

void GraphDesignerPage::changedAttrsScope(const AttrsType type, AttributesScope attrs)
{
    if (type == AttrsType::Edges) {
        m_edgeAttrScope = attrs;
    } else if (type == AttrsType::Nodes) {
        m_nodeAttrScope = attrs;
    }

    m_graphDesigner->slotUpdateAttrs();
}

void GraphDesignerPage::changedGraphAttrs(const int numNodes, PluginKey selectedGraphKey, GraphType graphType,
    QStringList& graphAttrHeader, QStringList& graphAttrValues, QString& error)
{
    m_numNodes = numNodes;
    m_graphAttrHeader = graphAttrHeader;
    m_graphAttrValues = graphAttrValues;
    m_graphType = graphType;
    m_selectedGraphKey = selectedGraphKey;

    m_graphDesigner->slotUpdateGraph(error);
}

}
