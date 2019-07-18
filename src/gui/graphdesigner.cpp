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

#include "core/include/attrsgenerator.h"
#include "core/include/enum.h"
#include "core/graphinputs.h"
#include "core/plugin.h"

#include "abstractgraph.h"
#include "graphdesigner.h"

namespace evoplex {

GraphDesigner::GraphDesigner(MainApp* mainApp, QMainWindow *parent)
    : QDockWidget(parent),
    m_mainApp(mainApp),
    m_innerWindow(new QMainWindow(this))
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

void GraphDesigner::initEmptyGraph()
{
    QStringList header;
    QStringList values;
    QString errorMsg;

    // Set inputs
    header << GENERAL_ATTR_EXPID << GENERAL_ATTR_EDGEATTRS << GENERAL_ATTR_GRAPHTYPE <<
        GENERAL_ATTR_NODES << GENERAL_ATTR_GRAPHID << GENERAL_ATTR_GRAPHVS;
    values << QString::number(m_curGraphId) << "" << "undirected" << "1" << "zeroEdges" << "1";
    
    auto graphInpts = GraphInputs::parse(m_mainApp, header, values, errorMsg);

    if (!errorMsg.isEmpty()) { qDebug() << errorMsg; }

    AttributesScope _attrs;

    AbstractGraph* a_g = dynamic_cast<AbstractGraph*>(graphInpts->graphPlugin()->create());
    GraphWidget* graphW = new GraphWidget(GraphWidget::Mode::Graph, a_g, _attrs, this);

    graphW->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_innerWindow->setCentralWidget(graphW);
}

GraphDesigner::~GraphDesigner()
{
}

} // evoplex
