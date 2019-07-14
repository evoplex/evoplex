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

#include <QSettings>
#include <QSlider>

#include "graphsettings.h"
#include "ui_graphsettings.h"
#include "maingui.h"
#include "graphview.h"

namespace evoplex {

GraphSettings::GraphSettings(ColorMapMgr* cMgr, ExperimentPtr exp, GraphView* parent)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
      m_ui(new Ui_GraphSettings),
      m_parent(parent),
      m_cMgr(cMgr),
      m_exp(exp)
{
    m_ui->setupUi(this);
    connect(m_exp.get(), SIGNAL(restarted()), SLOT(init()));

    connect(m_ui->nodesColor, SIGNAL(cmapUpdated(ColorMap*)),
            parent, SLOT(setNodeCMap(ColorMap*)));
    connect(m_ui->edgesColor, SIGNAL(cmapUpdated(ColorMap*)),
            parent, SLOT(setEdgeCMap(ColorMap*)));

    connect(m_ui->nodeScale, SIGNAL(valueChanged(int)), SLOT(slotNodeScale(int)));
    connect(m_ui->edgeScale, SIGNAL(valueChanged(int)), SLOT(slotEdgeScale(int)));
    connect(m_ui->edgeWidth, SIGNAL(valueChanged(int)), SLOT(slotEdgeWidth(int)));

    connect(m_ui->bOk, SIGNAL(pressed()), SLOT(close()));
    connect(m_ui->bRestore, SIGNAL(pressed()), SLOT(restoreSettings()));
    connect(m_ui->bSaveAsDefault, SIGNAL(pressed()), SLOT(saveAsDefault()));

    init();
}

GraphSettings::~GraphSettings()
{
    delete m_ui;
}

void GraphSettings::init()
{
    Q_ASSERT_X(m_exp->modelPlugin(), "GraphSettings",
               "tried to init the graph settings for a null model!");

    QSettings userPrefs;
    m_ui->nodeScale->setValue(userPrefs.value("graphSettings/nodeScale", 10).toInt());
    m_ui->edgeScale->setValue(userPrefs.value("graphSettings/edgeScale", 25).toInt());
    m_ui->edgeWidth->setValue(userPrefs.value("graphSettings/edgeWidth", 1).toInt());

    auto cmap = m_cMgr->defaultCMapKey();
    CMapKey n(userPrefs.value("graphSettings/nodeCMap", cmap.first).toString(),
              userPrefs.value("graphSettings/nodeCMapSize", cmap.second).toInt());
    m_ui->nodesColor->init(m_cMgr, n, m_exp->modelPlugin()->nodeAttrsScope());
    m_ui->nodesColor->setVisible(!m_exp->modelPlugin()->nodeAttrsScope().empty());

    CMapKey e(userPrefs.value("graphSettings/edgeCMap", cmap.first).toString(),
              userPrefs.value("graphSettings/edgeCMapSize", cmap.second).toInt());
    m_ui->edgesColor->init(m_cMgr, e, m_exp->modelPlugin()->edgeAttrsScope());
    m_ui->edgesColor->setVisible(!m_exp->modelPlugin()->edgeAttrsScope().empty());
}

void GraphSettings::restoreSettings()
{
    QSettings userPrefs;
    userPrefs.setValue("graphSettings/nodeScale", 10);
    userPrefs.setValue("graphSettings/edgeScale", 25);
    userPrefs.setValue("graphSettings/edgeWidth", 1);
    auto cmap = m_cMgr->defaultCMapKey();
    userPrefs.setValue("graphSettings/nodeCMap", cmap.first);
    userPrefs.setValue("graphSettings/nodeCMapSize", cmap.second);
    userPrefs.setValue("graphSettings/edgeCMap", cmap.first);
    userPrefs.setValue("graphSettings/edgeCMapSize", cmap.second);
    init();
}

void GraphSettings::saveAsDefault()
{
    QSettings userPrefs;
    userPrefs.setValue("graphSettings/nodeScale", nodeScale());
    userPrefs.setValue("graphSettings/edgeScale", edgeScale());
    userPrefs.setValue("graphSettings/edgeWidth", edgeWidth());
    userPrefs.setValue("graphSettings/nodeCMap", nodeColorSelector()->cmapName());
    userPrefs.setValue("graphSettings/nodeCMapSize", nodeColorSelector()->cmapSize());
    userPrefs.setValue("graphSettings/edgeCMap", edgeColorSelector()->cmapName());
    userPrefs.setValue("graphSettings/edgeCMapSize", edgeColorSelector()->cmapSize());
}

void GraphSettings::slotNodeScale(int v)
{
    m_parent->setNodeScale(v);
    m_ui->nodeScale->setToolTip(QString::number(v));
}

void GraphSettings::slotEdgeScale(int v)
{
    m_parent->setEdgeScale(v);
    m_ui->edgeScale->setToolTip(QString::number(v));
}

void GraphSettings::slotEdgeWidth(int v)
{
    m_parent->setEdgeWidth(v);
    m_ui->edgeWidth->setToolTip(QString::number(v));
}

int GraphSettings::nodeScale() const
{
    return m_ui->nodeScale->value();
}

int GraphSettings::edgeScale() const
{
    return m_ui->edgeScale->value();
}

int GraphSettings::edgeWidth() const
{
    return m_ui->edgeWidth->value();
}

AttrColorSelector* GraphSettings::nodeColorSelector() const
{
    return m_ui->nodesColor;
}

AttrColorSelector* GraphSettings::edgeColorSelector() const
{
    return m_ui->edgesColor;
}

} // evoplex
