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
#include <QMessageBox>
#include <QHBoxLayout>
#include <QSettings>

#include "fontstyles.h"
#include "graphattrsdlg.h"
#include "graphdesignerpage.h"
#include "graphdesigner.h"
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
    connect(m_ui->acGraphGen, &QAction::triggered, [this]() {
        if (!this->m_nodeAttrScope.isEmpty()) {
            this->slotGraphGen();
        } else {
            QMessageBox::warning(this, "Graph Generator",
            "Make sure you have set valid node attributes before attempting to open the graph generator");
        }
    });
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
    
    QString errstrng;

    changedGraphAttrs(m_numNodes, m_selectedGraphKey, m_graphType, m_graphAttrHeader, m_graphAttrValues, errstrng);
}

void GraphDesignerPage::changedGraphAttrs(const int numNodes, PluginKey selectedGraphKey, GraphType graphType,
    QStringList& graphAttrHeader, QStringList& graphAttrValues, QString& error)
{
    m_numNodes = numNodes;
    m_graphAttrHeader = graphAttrHeader;
    m_graphAttrValues = graphAttrValues;
    m_graphType = graphType;
    m_selectedGraphKey = selectedGraphKey;

    if (m_numNodes > 0) {
        //TODO: No need to update both the edge and node attributes each time either one is updated
        m_graphDesigner->setup(parseInputs(), m_nodeAttrScope, m_edgeAttrScope);
    }
}

GraphInputsPtr GraphDesignerPage::parseInputs()
{
    QStringList header;
    QStringList values;
    QString errorMsg;

    header << GENERAL_ATTR_EXPID;
    values << QString::number(0);

    //TODO: Edge attributes widget
    header << GENERAL_ATTR_EDGEATTRS;
    values << "";

    header << GENERAL_ATTR_NODES;
    values << QString::number(m_numNodes);

    header << GENERAL_ATTR_GRAPHTYPE;

    if (m_graphType == GraphType::Undirected) {
        values << "undirected";
    } else if (m_graphType == GraphType::Directed) {
        values << "directed";
    } else {
        values << "invalid";
    }

    header += graphAttrHeader();
    values += graphAttrValues();

    header << GENERAL_ATTR_GRAPHID << GENERAL_ATTR_GRAPHVS;
    values << selectedGraphKey().first << QString::number(selectedGraphKey().second);

    QString errorstrng;
    auto inputs = GraphInputs::parse(m_mainApp, header, values, errorstrng);
    if (!inputs || !errorstrng.isEmpty()) {
        return nullptr;
    }

    return inputs;
}

} // evoplex
