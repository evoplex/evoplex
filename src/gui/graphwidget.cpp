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
#include "graphtitlebar.h"

namespace evoplex {

GraphWidget::GraphWidget(Mode mode, ColorMapMgr* cMgr,
                         ExperimentPtr exp, ExperimentWidget* parent)
    : QDockWidget(parent),
      m_view(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    GraphTitleBar* titleBar = new GraphTitleBar(exp.get(), this);
    setTitleBarWidget(titleBar);

    if (mode == Mode::Graph) {
        m_view = new GraphView(cMgr, exp, this);
        setWindowTitle("Graph");
    } else {
        m_view = new GridView(cMgr, exp, this);
        setWindowTitle("Grid");
    }
    setWidget(m_view);
    connect(m_view, SIGNAL(updateWidgets(bool)),
            SIGNAL(updateWidgets(bool)));

    connect(titleBar, &GraphTitleBar::openSettingsDlg,
            [this]() { m_view->openSettings(); });
    connect(titleBar, SIGNAL(trialSelected(quint16)),
            m_view, SLOT(setTrial(quint16)));

    // the widget might not be repaint after a drag and drop
    connect(this, &QDockWidget::dockLocationChanged,
            [this](){ m_view->update(); });
}

GraphWidget::~GraphWidget()
{
    delete m_view;
}

void GraphWidget::updateView(bool forceUpdate)
{
    if (m_view) m_view->updateView(forceUpdate);
}

} // evoplex
