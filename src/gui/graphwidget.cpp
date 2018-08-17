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

#include "graphwidget.h"
#include "basegraphgl.h"
#include "graphview.h"
#include "gridview.h"
#include "titlebar.h"

namespace evoplex {

GraphWidget::GraphWidget(Mode mode, MainGUI* mainGUI,
                         ExperimentPtr exp, ExperimentWidget* parent)
    : QDockWidget(parent),
      m_settingsDlg(new GraphSettings(mainGUI, exp, this)),
      m_graph(nullptr)
{
    TitleBar* titleBar = new TitleBar(exp.get(), this);
    setTitleBarWidget(titleBar);

    if (mode == Mode::Graph) {
        m_graph = new GraphView(exp, this);
    } else {
        m_graph = new GridView(exp, this);
    }
    setWidget(m_graph);
    connect(m_graph, SIGNAL(updateWidgets(bool)), SIGNAL(updateWidgets(bool)));

    connect(titleBar, SIGNAL(openSettingsDlg()), m_settingsDlg, SLOT(show()));
    connect(titleBar, SIGNAL(trialSelected(quint16)), m_graph, SLOT(setTrial(quint16)));
}

GraphWidget::~GraphWidget()
{
}

void GraphWidget::updateView(bool forceUpdate)
{
    if (m_graph) m_graph->updateView(forceUpdate);
}

} // evoplex
