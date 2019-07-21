/**
*  This file is part of Evoplex.
*
*  Evoplex is a multi-agent system for networks.
*  Copyright (C) 2019 - Eleftheria Chatziargyriou <ele.hatzy@gmail.com>
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
#include <QDialog>
#include <QSet>
#include <QSizePolicy>
#include <QMessageBox>

#include "core/include/attributerange.h"
#include "core/graphplugin.h"
#include "core/plugin.h"

#include "attrwidget.h"
#include "graphgendlg.h"
#include "ui_graphgendlg.h"

namespace evoplex {

GraphGenDlg::GraphGenDlg(GraphDesignerPage* parent, MainGUI* mainGUI)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
    m_ui(new Ui_GraphGenDlg),
    m_graphPage(parent),
    m_mainGUI(mainGUI)
{
    setWindowModality(Qt::ApplicationModal);

    m_ui->setupUi(this);
    setVisible(true);
    
    m_ui->graphType->insertItem(0, "--");
    
    int i = 0;
    for (Plugin* p : m_mainGUI->mainApp()->plugins()) {
        if (p->type() == PluginType::Graph) {
            m_ui->graphType->insertItem(++i, p->title());
            m_plugins.insert(i, p->key());
        }
    }

    connect(m_ui->ok, SIGNAL(clicked()), SLOT(slotSaveGraphGen()));
    connect(m_ui->graphType, SIGNAL(currentIndexChanged(int)), SLOT(slotGraphSelected(int)));
    connect(m_ui->cancel, SIGNAL(clicked()), SLOT(close()));
};

void GraphGenDlg::parseAttrs(QString& error)
{
    if (m_selectedGraphKey == PluginKey()) {
        error = "Please select a valid graph id";
        return;
    }

    m_numNodes = m_ui->numNodes->value();

    for (auto it = m_attrWidgets.cbegin(); it != m_attrWidgets.cend(); ++it) {
        m_attrHeader << m_selectedGraphKey.first + "_" + it.value()->attrName();
        m_attrValues << it.value()->value().toQString('g', 8);
    }
}

void GraphGenDlg::slotSaveGraphGen()
{
    QString errstrng;
    parseAttrs(errstrng);

    if (!errstrng.isEmpty()) {
        QMessageBox::warning(this, "Graph Generator", "Error: " + errstrng);
        return;
    }

    m_graphPage->changedGraphAttrs(m_numNodes, m_attrHeader, m_attrValues);
    close();
}

void GraphGenDlg::slotGraphSelected(int grId) 
{
    // Clear list widget entries
    while (m_ui->attrForm->count() > 0) {
        auto item = m_ui->attrForm->takeAt(0);
        auto widget = item->widget();
        delete widget;
        delete item;
    }

    if (grId == 0) {
        m_selectedGraphKey = PluginKey();
        return;
    }

    m_selectedGraphKey = m_plugins.value(grId);
    const GraphPlugin* graph = m_mainGUI->mainApp()->graph(m_selectedGraphKey);
    
    QHash<QString, AttributeRangePtr>::const_iterator it = graph->pluginAttrsScope().begin();

    while (it != graph->pluginAttrsScope().end()) {
        AttrWidget* attrW = new AttrWidget(it.value(), this);        
        m_ui->attrForm->addRow(it.key(), attrW);
        m_attrWidgets.insert(it.key(), attrW);
        
        ++it;
    }
}

GraphGenDlg::~GraphGenDlg()
{
    delete m_ui;
}


}