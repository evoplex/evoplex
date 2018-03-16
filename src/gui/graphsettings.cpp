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
    , m_exp(exp)
    , m_modelPlugin(nullptr)
    , m_cmMgr(mainGUI->colorMapMgr())
{
    m_ui->setupUi(this);

    connect(m_ui->agentAttr, SIGNAL(currentIndexChanged(int)), SLOT(setAgentAttr(int)));
    connect(m_ui->agentCMapName, SIGNAL(currentIndexChanged(QString)), SLOT(slotAgentCMapName(QString)));
    connect(m_ui->agentCMapSize, SIGNAL(currentIndexChanged(int)), SLOT(updateAgentCMap()));

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

    // agent stuff
    m_ui->agentAttr->clear();
    m_ui->agentCMapName->clear();
    for (QString attrName : m_modelPlugin->agentAttrNames()) {
        m_ui->agentAttr->addItem(attrName);
    }
    m_ui->agentCMapName->insertItems(0, m_cmMgr->names());
    m_ui->agentCMapName->setCurrentText(m_cmMgr->defaultColorMap().first);
    slotAgentCMapName(m_cmMgr->defaultColorMap().first); // fill sizes
    m_ui->agentCMapSize->setCurrentText(QString::number(m_cmMgr->defaultColorMap().second));
    updateAgentCMap();

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

void GraphSettings::updateAgentCMap()
{
    const ValueSpace* valSpace = m_modelPlugin->agentAttrSpace(m_ui->agentAttr->currentText());
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
