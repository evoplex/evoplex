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

#include "core/trial.h"

#include "graphwidget.h"
#include "basegraphgl.h"
#include "graphview.h"
#include "gridview.h"
#include "graphtitlebar.h"
#include "graphsettings.h"
#include "graphdesignerpage.h"
#include "gridsettings.h"

namespace evoplex {

// (cardinot) TODO: this class should be refactored to remove the Experiment dependency
GraphWidget::GraphWidget(Mode mode, ColorMapMgr* cMgr, ExperimentPtr exp, QWidget* parent)
    : GraphWidget(mode, nullptr, AttributesScope(), AttributesScope(), parent)
{
    m_exp = exp;
    Q_ASSERT(m_exp && m_view); // TODO

    Q_ASSERT_X(!m_exp->autoDeleteTrials(), "GraphWidget",
               "tried to build a GraphWidget for a experiment that will be auto-deleted!");

    GraphTitleBar* titleBar = new GraphTitleBar(exp.get(), this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(trialSelected(int)), SLOT(setTrial(int)));
    connect(titleBar, &GraphTitleBar::openSettingsDlg,
            [this]() { m_settingsDlg->show(); });

    // it triggers a timer that needs to be exec in the main thread
    // thus, we need to use queuedconnection here
    connect(m_exp.get(), SIGNAL(trialCreated(quint16)), SLOT(setTrial(quint16)), Qt::QueuedConnection);

    connect(m_exp.get(), SIGNAL(statusChanged(Status)), m_view, SLOT(slotStatusChanged(Status)));

    connect(exp.get(), SIGNAL(restarted()), SLOT(slotRestarted()));

    if (mode == Mode::Graph) {
        auto view = qobject_cast<GraphView*>(m_view);
        auto graphSettings = new GraphSettings(cMgr, m_exp, view);
        view->setNodeScale(graphSettings->nodeScale());
        view->setEdgeScale(graphSettings->edgeScale());
        m_settingsDlg = graphSettings;
    } else {
        auto view = qobject_cast<GridView*>(m_view);
        auto gridSettings = new GridSettings(cMgr, m_exp, view);
        m_settingsDlg = gridSettings;
    }

    if (!m_exp->trials().empty()) {
        setTrial(0); // init with the first trial
    }
}

GraphWidget::GraphWidget(Mode mode, AbstractGraph* graph, AttributesScope nodeAttrsScope, AttributesScope edgeAttrsScope, QWidget* parent)
    : QDockWidget(parent),
      m_view(nullptr),
      m_currTrial(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ColorMapMgr* cMgr = new ColorMapMgr;

    if (mode == Mode::Graph) {
        m_view = new GraphView(graph, nodeAttrsScope, this);
        setWindowTitle("Graph");
        auto view = qobject_cast<GraphView*>(m_view);
        auto graphSettings = new GraphSettings(cMgr, nodeAttrsScope, edgeAttrsScope, view);
        m_settingsDlg = graphSettings;
    } else {
        m_view = new GridView(graph, nodeAttrsScope, this);
        setWindowTitle("Grid");
    }
    setWidget(m_view);
    connect(m_view, SIGNAL(updateWidgets(bool)), SIGNAL(updateWidgets(bool)));

    // the widget might not be repaint after a drag and drop
    connect(this, &QDockWidget::dockLocationChanged,
            [this](){ m_view->update(); });
}

GraphWidget::~GraphWidget()
{
    if (m_exp) m_exp->disconnect(this); // important to avoid triggering statusChanged()
    delete m_view;
    m_exp = nullptr;
}

void GraphWidget::slotOpenSettings()
{
    m_settingsDlg->show();
}

void GraphWidget::updateView(bool forceUpdate)
{  
    if (!m_currTrial || !m_view || !m_currTrial->model()
            || (!forceUpdate && m_currTrial->step() == m_view->currStep())) {
        return;
    }

    m_view->setCurrentStep(m_currTrial->step());
    m_view->update();
}

void GraphWidget::slotRestarted()
{
    if (m_exp && m_exp->autoDeleteTrials()) {
        close();
    } else {
        m_view->slotRestarted();
    }
}

void GraphWidget::setTrial(quint16 trialId)
{
    if (auto trial = m_exp->trial(trialId)) {
        if (!m_currTrial || m_currTrial->id() == trialId) {
            m_view->setCurrentStep(trial->step());
        }
        m_currTrial = trial;
        m_view->setup(trial->graph(), m_exp->modelPlugin()->nodeAttrsScope());
    } else {
        Q_ASSERT(false); // this should never happen
    }
}

} // evoplex
