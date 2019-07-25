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

#include <QDebug>
#include <QVBoxLayout>
#include <QString>
#include <QStringList>

#include "core/include/abstractmodel.h"
#include "core/include/attributerange.h"
#include "core/include/enum.h"
#include <core/include/nodes.h>
#include <core/nodes_p.h>

#include "core/graphplugin.h"
#include "core/plugin.h"

#include "colormap.h"
#include "graphdesigner.h"
#include "prg.h"

namespace evoplex {

GraphDesigner::GraphDesigner(MainGUI* mainGUI, GraphDesignerPage *parent)
    : QDockWidget(parent),
    m_mainGUI(mainGUI),
    m_mainApp(mainGUI->mainApp()),
    m_innerWindow(new QMainWindow(this)),
    m_curGraph(nullptr),
    m_parent(parent)
{
    setObjectName("GraphDesigner");

    m_innerWindow->setObjectName("graphDesignerViewWindow");
    m_innerWindow->setDockNestingEnabled(false);

    QVBoxLayout* layout = new QVBoxLayout(new QWidget(this));
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_innerWindow);
    setWidget(layout->parentWidget());

    m_curGraphId = 0;
    initEmptyGraph();
}

void GraphDesigner::slotOpenSettings()
{
    m_curGraph->slotOpenSettings();
}

void GraphDesigner::slotUpdateAttrs()
{
    m_curGraph = new GraphWidget(GraphWidget::Mode::Graph, m_abstrGraph, m_parent->nodeAttributesScope(), m_parent->edgeAttributesScope(), this);
}

void GraphDesigner::slotUpdateGraph(QString& error)
{
    GraphInputsPtr inputs = parseInputs(error);

    if (!error.isEmpty()) {
        error.prepend("Error when parsing inputs:\n");
        return;
    }

    PRG* prg = new PRG(0);
    AttrsGeneratorPtr edgeGen = AttrsGenerator::parse(m_parent->edgeAttributesScope(), QString::number(m_parent->numNodes()), error);

    if (!error.isEmpty()) {
        error.prepend("Error when parsing edge attributes:\n");
        return;
    }

    Nodes nodes = NodesPrivate::fromCmd(QString::number(m_parent->numNodes()), m_parent->nodeAttributesScope(), GraphType::Undirected, error);

    if (!error.isEmpty()) {
        error.prepend("Error when creating nodes:\n");
        return;
    }

    m_abstrGraph = dynamic_cast<AbstractGraph*>(inputs->graphPlugin()->create());
    m_abstrGraph->setup(QString::number(m_curGraphId), m_parent->graphType(), *prg,
        std::move(edgeGen), nodes, *inputs->graph());
    
    m_curGraph = new GraphWidget(GraphWidget::Mode::Graph, m_abstrGraph, m_parent->nodeAttributesScope(), m_parent->edgeAttributesScope(), this);
}

GraphInputsPtr GraphDesigner::parseInputs(QString& error)
{
    QStringList header;
    QStringList values;
    QString errorMsg;

    header << GENERAL_ATTR_EXPID;
    values << QString::number(m_curGraphId);

    //TODO: Edge attributes widget
    header << GENERAL_ATTR_EDGEATTRS;
    values << "";

    header << GENERAL_ATTR_NODES;
    values << QString::number(m_parent->numNodes());

    header << GENERAL_ATTR_GRAPHTYPE;

    if (m_parent->graphType() == GraphType::Undirected) {
        values << "undirected";
    } else if (m_parent->graphType() == GraphType::Directed) {
        values << "directed";
    } else {
        values << "invalid";
    }

    header += m_parent->graphAttrHeader();
    values += m_parent->graphAttrValues();

    header << GENERAL_ATTR_GRAPHID << GENERAL_ATTR_GRAPHVS;
    values << m_parent->selectedGraphKey().first << QString::number(m_parent->selectedGraphKey().second);

    QString errorstrng;
    auto inputs = GraphInputs::parse(m_mainApp, header, values, errorstrng);
    if (!inputs || !errorstrng.isEmpty()) {
        return nullptr;
    }

    return inputs;
}

void GraphDesigner::initEmptyGraph()
{
    QStringList header;
    QStringList values;
    QString errorMsg;

    m_abstrGraph = new BaseAbstractGraph();
    AttributesScope attrs;
    m_curGraph = new GraphWidget(GraphWidget::Mode::Graph, m_abstrGraph, attrs, attrs, this);

    m_curGraph->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_innerWindow->setCentralWidget(m_curGraph);
}

GraphDesigner::~GraphDesigner()
{
}

} // evoplex
