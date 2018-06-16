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

#include "graphsettings.h"
#include "ui_graphsettings.h"

namespace evoplex
{

GraphSettings::GraphSettings(MainGUI* mainGUI, Experiment* exp, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui_GraphSettings)
    , m_exp(exp)
    , m_modelPlugin(nullptr)
    , m_cmMgr(mainGUI->colorMapMgr())
{
    m_ui->setupUi(this);

    connect(m_ui->nodeAttr, SIGNAL(currentIndexChanged(int)), SLOT(setNodeAttr(int)));
    connect(m_ui->nodeCMapName, SIGNAL(currentIndexChanged(QString)), SLOT(slotNodeCMapName(QString)));
    connect(m_ui->nodeCMapSize, SIGNAL(currentIndexChanged(int)), SLOT(updateNodeCMap()));

    connect(m_ui->edgeAttr, SIGNAL(currentIndexChanged(int)), SLOT(setEdgeAttr(int)));
    connect(m_ui->edgeCMapName, SIGNAL(currentIndexChanged(QString)), SLOT(slotEdgeCMapName(QString)));
    connect(m_ui->edgeCMapSize, SIGNAL(currentIndexChanged(int)), SLOT(updateEdgeCMap()));

    connect(m_exp, SIGNAL(restarted()), SLOT(init()));
    init();
}

GraphSettings::~GraphSettings()
{
    delete m_ui;
}

void GraphSettings::init()
{
    Q_ASSERT(m_exp->modelPlugin());
    if (m_exp->modelPlugin() == m_modelPlugin) {
        return;
    }

    m_modelPlugin = m_exp->modelPlugin();

    // node stuff
    m_ui->nodeAttr->clear();
    m_ui->nodeCMapName->clear();
    for (QString attrName : m_modelPlugin->nodeAttrNames()) {
        m_ui->nodeAttr->addItem(attrName);
    }
    m_ui->nodeCMapName->insertItems(0, m_cmMgr->names());
    m_ui->nodeCMapName->setCurrentText(m_cmMgr->defaultColorMap().first);
    slotNodeCMapName(m_cmMgr->defaultColorMap().first); // fill sizes
    m_ui->nodeCMapSize->setCurrentText(QString::number(m_cmMgr->defaultColorMap().second));
    updateNodeCMap();

    // edge stuff
    m_ui->edgeAttr->clear();
    m_ui->edgeCMapName->clear();
    for (QString attrName : m_modelPlugin->edgeAttrNames()) {
        m_ui->edgeAttr->addItem(attrName);
    }
    m_ui->edgeCMapName->insertItems(0, m_cmMgr->names());
    m_ui->edgeCMapName->setCurrentText(m_cmMgr->defaultColorMap().first);
    slotEdgeCMapName(m_cmMgr->defaultColorMap().first); // fill sizes
    m_ui->edgeCMapSize->setCurrentText(QString::number(m_cmMgr->defaultColorMap().second));
    updateEdgeCMap();
}

void GraphSettings::updateNodeCMap()
{
    const ValueSpace* valSpace = m_modelPlugin->nodeAttrSpace(m_ui->nodeAttr->currentText());
    ColorMap* cmap = ColorMap::create(valSpace,
            m_cmMgr->colors(m_ui->nodeCMapName->currentText(),
                            m_ui->nodeCMapSize->currentText().toInt()));
    m_nodeCMap = cmap;
    emit (nodeCMapUpdated(m_nodeCMap));
}

void GraphSettings::slotNodeCMapName(const QString &name)
{
    m_ui->nodeCMapSize->blockSignals(true);
    m_ui->nodeCMapSize->clear();
    m_ui->nodeCMapSize->insertItems(0, m_cmMgr->sizes(name));
    m_ui->nodeCMapSize->blockSignals(false);
    updateNodeCMap();
}

void GraphSettings::setNodeAttr(int attrIdx)
{
    emit (nodeAttrUpdated(attrIdx));
    updateNodeCMap();
}

int GraphSettings::nodeAttr() const
{
    return m_ui->nodeAttr->currentIndex();
}

/******/

void GraphSettings::updateEdgeCMap()
{
    const ValueSpace* valSpace = m_modelPlugin->edgeAttrSpace(m_ui->edgeAttr->currentText());
    ColorMap* cmap = ColorMap::create(valSpace,
            m_cmMgr->colors(m_ui->edgeCMapName->currentText(),
                            m_ui->edgeCMapSize->currentText().toInt()));
    m_edgeCMap = cmap;
    emit (edgeCMapUpdated(m_edgeCMap));
}

void GraphSettings::slotEdgeCMapName(const QString &name)
{
    m_ui->edgeCMapSize->blockSignals(true);
    m_ui->edgeCMapSize->clear();
    m_ui->edgeCMapSize->insertItems(0, m_cmMgr->sizes(name));
    m_ui->edgeCMapSize->blockSignals(false);
    updateEdgeCMap();
}

void GraphSettings::setEdgeAttr(int attrIdx)
{
    emit (edgeAttrUpdated(attrIdx));
    updateEdgeCMap();
}

int GraphSettings::edgeAttr() const
{
    return m_ui->edgeAttr->currentIndex();
}

} // evoplex
