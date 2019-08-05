/**
*  This file is part of Evoplex.
*
*  Evoplex is a multi-agent system for networks.
*  Copyright (C) 2019
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
#include <QFileDialog>
#include <QVBoxLayout>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QProgressDialog>

#include "core/include/abstractmodel.h"
#include "core/include/attributerange.h"
#include "core/include/enum.h"
#include "core/include/nodes.h"
#include "core/nodes_p.h"

#include "core/graphplugin.h"
#include "core/plugin.h"

#include "basegraphgl.h"
#include "colormap.h"
#include "graphdesigner.h"
#include "prg.h"

namespace evoplex {

GraphDesigner::GraphDesigner(MainGUI* mainGUI, GraphDesignerPage* parent)
    : QDockWidget(parent),
      m_mainGUI(mainGUI),
      m_mainApp(mainGUI->mainApp()),
      m_innerWindow(new QMainWindow(this)),
      m_curGraph(new GraphWidget(GraphWidget::Mode::Graph, mainGUI->colorMapMgr(), this)),
      m_abstrGraph(nullptr),
      m_prg(new PRG(0))
{
    setObjectName("GraphDesigner");

    m_curGraph->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_innerWindow->setCentralWidget(m_curGraph);

    m_innerWindow->setObjectName("graphDesignerViewWindow");
    m_innerWindow->setDockNestingEnabled(false);

    QVBoxLayout* layout = new QVBoxLayout(new QWidget(this));
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_innerWindow);
    
    m_curGraph->setViewInspector(parent->fullInspector());
    connect(m_curGraph->view(), SIGNAL(nodeSelected(Node&)), parent->fullInspector(), SLOT(slotSelectedNode(Node&)));
    connect(m_curGraph->view(), SIGNAL(clearedSelected()), parent->fullInspector(), SLOT(slotClear()));

    setWidget(layout->parentWidget());
}

GraphDesigner::~GraphDesigner()
{
    delete m_prg;
}

void GraphDesigner::setup(GraphInputsPtr inputs, AttributesScope nodeAttrsScope, AttributesScope edgeAttrsScope)
{
    QString error;

    const auto numNodes = inputs->general(GENERAL_ATTR_NODES).toQString();
    AttrsGeneratorPtr edgeGen = AttrsGenerator::parse(edgeAttrsScope, numNodes, error);
    if (!error.isEmpty()) {
        error.prepend("Error when parsing edge attributes:\n");
        return;
    }

    auto graphType = _enumFromString<GraphType>(inputs->general(GENERAL_ATTR_GRAPHTYPE).toQString());
    Nodes nodes = NodesPrivate::fromCmd(numNodes, nodeAttrsScope, graphType, error);
    if (!error.isEmpty()) {
        error.prepend("Error when creating nodes:\n");
        return;
    }

    m_abstrGraph = dynamic_cast<AbstractGraph*>(inputs->graphPlugin()->create());
    if (!m_abstrGraph || !m_abstrGraph->setup("0", graphType, *m_prg, std::move(edgeGen), nodes, *inputs->graph())) {
        error.prepend("Could not initialize graph:\n");
        return;
    }

    m_abstrGraph->reset();

    m_curGraph->setup(m_abstrGraph, nodeAttrsScope, edgeAttrsScope);
}

void GraphDesigner::slotOpenSettings()
{
    m_curGraph->slotOpenSettings();
}

bool GraphDesigner::readyToExport()
{
    if (m_abstrGraph->nodes().empty()) {
        QMessageBox::warning(this, "Exporting nodes",
            "Could not export the set of nodes.\n"
            "Please, make sure there are nodes to be exported!");
        return false;
    }

    return true;
}

void GraphDesigner::slotChangeSelectionMode(SelectionMode m)
{
    m_curGraph->changeSelectionMode(m);
}

void GraphDesigner::slotExportNodes()
{
    if (!readyToExport()) {
        return;
    }

    QString path = "";
    path = QFileDialog::getSaveFileName(this, "Export Nodes", path, "Text Files (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    QProgressDialog progressDlg("Exporting nodes", QString(), 0, m_abstrGraph->numNodes(), this);
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.setValue(0);
    std::function<void(int)> progress = [&progressDlg](int p) { progressDlg.setValue(p); };

    if (NodesPrivate::saveToFile(m_abstrGraph->nodes(), path, progress)) {
        QMessageBox::information(this, "Exporting nodes",
            "The set of nodes was saved successfully!\n" + path);
    }
    else {
        QMessageBox::warning(this, "Exporting nodes",
            "ERROR! Unable to save the set of nodes at:\n"
            + path + "\nPlease, make sure this directory is writable.");
    }
}

} // evoplex
