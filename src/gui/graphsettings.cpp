/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "graphsettings.h"
#include "ui_graphsettings.h"

namespace evoplex
{

GraphSettings::GraphSettings(MainGUI* mainGUI, Experiment* exp, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui_GraphSettings)
    , m_cmMgr(mainGUI->colorMapMgr())
    , m_agentAttrSpace(exp->modelPlugin()->agentAttrSpace())
    , m_edgeAttrSpace(exp->modelPlugin()->agentAttrSpace())
{
    m_ui->setupUi(this);

    // agent stuff

    for (QString attrName : exp->modelPlugin()->agentAttrNames()) {
        m_ui->agentAttr->addItem(attrName);
    }
    connect(m_ui->agentAttr, SIGNAL(currentIndexChanged(int)), SLOT(setAgentAttr(int)));

    m_ui->agentCMapName->insertItems(0, m_cmMgr->names());
    m_ui->agentCMapName->setCurrentText(m_cmMgr->defaultColorMap().first);
    slotAgentCMapName(m_cmMgr->defaultColorMap().first); // fill sizes
    m_ui->agentCMapSize->setCurrentText(QString::number(m_cmMgr->defaultColorMap().second));
    connect(m_ui->agentCMapName, SIGNAL(currentIndexChanged(QString)), SLOT(slotAgentCMapName(QString)));
    connect(m_ui->agentCMapSize, SIGNAL(currentIndexChanged(int)), SLOT(updateAgentCMap()));
    updateAgentCMap();

    // edge stuff

    for (QString attrName : exp->modelPlugin()->edgeAttrNames()) {
        m_ui->edgeAttr->addItem(attrName);
    }
    connect(m_ui->edgeAttr, SIGNAL(currentIndexChanged(int)), SLOT(setEdgeAttr(int)));

    m_ui->edgeCMapName->insertItems(0, m_cmMgr->names());
    m_ui->edgeCMapName->setCurrentText(m_cmMgr->defaultColorMap().first);
    slotEdgeCMapName(m_cmMgr->defaultColorMap().first); // fill sizes
    m_ui->edgeCMapSize->setCurrentText(QString::number(m_cmMgr->defaultColorMap().second));
    connect(m_ui->edgeCMapName, SIGNAL(currentIndexChanged(QString)), SLOT(slotEdgeCMapName(QString)));
    connect(m_ui->edgeCMapSize, SIGNAL(currentIndexChanged(int)), SLOT(updateEdgeCMap()));
    updateEdgeCMap();
}

GraphSettings::~GraphSettings()
{
    delete m_ui;
}

void GraphSettings::updateAgentCMap()
{
    const ValueSpace* valSpace = m_agentAttrSpace.value(m_ui->agentAttr->currentText());
    ColorMap* cmap = ColorMap::create(valSpace,
            m_cmMgr->colors(m_ui->agentCMapName->currentText(),
                            m_ui->agentCMapSize->currentText().toInt()));
    m_agentCMap = cmap;
    emit (agentCMapUpdated(m_agentCMap));
}

void GraphSettings::slotAgentCMapName(const QString &name)
{
    m_ui->agentCMapSize->blockSignals(true);
    m_ui->agentCMapSize->clear();
    m_ui->agentCMapSize->insertItems(0, m_cmMgr->sizes(name));
    m_ui->agentCMapSize->blockSignals(false);
    updateAgentCMap();
}

void GraphSettings::setAgentAttr(int attrIdx)
{
    emit (agentAttrUpdated(attrIdx));
    updateAgentCMap();
}

int GraphSettings::agentAttr() const
{
    return m_ui->agentAttr->currentIndex();
}

/******/

void GraphSettings::updateEdgeCMap()
{
    const ValueSpace* valSpace = m_edgeAttrSpace.value(m_ui->edgeAttr->currentText());
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
