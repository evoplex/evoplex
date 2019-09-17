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
#include <QDockWidget>

#include "fullinspector.h"
#include "ui_fullinspector.h"

namespace evoplex {

FullInspector::FullInspector(QWidget* parent)
    : QDockWidget(parent),
      m_ui(new Ui_FullInspector),
      m_parent(parent)
{
    m_ui->setupUi(this);
    m_ui->inspectorContents->hide();

    connect(m_ui->bdelete, SIGNAL(clicked()), SLOT(slotDelete()));
}

FullInspector::~FullInspector() 
{
    delete m_ui;
    m_nodeAttrWidgets.clear();
    m_edgeAttrWidgets.clear();
}

void FullInspector::hideLayout(QFormLayout* layout) {
    for (int i = 0; i < layout->count(); ++i) {
        layout->itemAt(i)->widget()->hide();
    }
}

void FullInspector::showLayout(QFormLayout* layout) {
    for (int i = 0; i < layout->count(); ++i) {
        layout->itemAt(i)->widget()->show();
    }
}

void FullInspector::slotClear() {
    m_ui->ids->clear();

    m_ui->textMsg->show();
    m_ui->inspectorContents->hide();
    m_selectedNodes.clear();
    m_selectedEdges.clear();
}

void FullInspector::slotChangeNodeAttrsScope(AttributesScope& attrScope) {
    slotChangeAttrScope(attrScope, m_nodeAttrWidgets, m_ui->nodeAttrs);
}

void FullInspector::slotChangeEdgeAttrsScope(AttributesScope& attrScope) {
    slotChangeAttrScope(attrScope, m_edgeAttrWidgets, m_ui->edgeAttrs);
}

void FullInspector::slotChangeAttrScope(AttributesScope& attrScope, std::vector<std::shared_ptr<AttrWidget>>& attrWidget, QFormLayout* lattrs)
{
    while (lattrs->count()) {
        auto item = lattrs->takeRow(0);
        delete item.labelItem->widget();
        delete item.labelItem;
        delete item.fieldItem;
    }

    attrWidget.clear();
    attrWidget.resize(static_cast<size_t>(attrScope.size()));

    for (auto attrRange : attrScope) {
        auto aw = std::make_shared<AttrWidget>(attrRange, nullptr);
        aw->setToolTip(attrRange->attrRangeStr());
        int aId = aw->id();
        if (m_edgeAttrWidgets == attrWidget) {
            connect(aw.get(), &AttrWidget::valueChanged, [this, aId]() { attrEdgeValueChanged(aId); });
        } else {
            connect(aw.get(), &AttrWidget::valueChanged, [this, aId]() { attrNodeValueChanged(aId); });
        }
        
        attrWidget.at(attrRange->id()) = aw;
        lattrs->insertRow(attrRange->id(), attrRange->attrName(), aw.get());

        QWidget* l = lattrs->labelForField(aw.get());
        l->setToolTip(attrRange->attrName());
        l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        l->setMinimumWidth(m_ui->lattrs->minimumWidth());
    }
}

void FullInspector::slotDelete()
{
    m_ui->ids->clear();
    if (m_selectedNodes.size() > 0) {
        m_selectedNodes.clear();
        emit(deleteNodes());
    } else if (m_selectedEdges.size() > 0) {
        m_selectedEdges.clear();
        emit(deleteEdges());
    }
}

void FullInspector::attrNodeValueChanged(int attrId) const
{
    std::shared_ptr<AttrWidget> aw;
    try { aw = m_nodeAttrWidgets.at(attrId); }
    catch (std::out_of_range) { return; }

    for (auto it : m_selectedNodes) {
        Value v = aw->validate();
        if (v.isValid()) {
            it.second.setAttr(aw->id(), v);
        } else {
            aw->blockSignals(true);
            aw->setValue(it.second.attr(aw->id()));
            aw->blockSignals(false);
        }
    }
}

void FullInspector::attrEdgeValueChanged(int attrId) const
{
    std::shared_ptr<AttrWidget> aw;
    try { aw = m_edgeAttrWidgets.at(attrId); }
    catch (std::out_of_range) { return; }

    for (auto it : m_selectedEdges) {
        Value v = aw->validate();
        if (v.isValid()) {
            it.second.setAttr(aw->id(), v);
        } else {
            aw->blockSignals(true);
            aw->setValue(it.second.attr(aw->id()));
            aw->blockSignals(false);
        }
    }
}

void FullInspector::slotSelectedNode(const Node& node)
{
    if (m_selectedEdges.size() > 0) {
        slotClear();
    } 
    
    if (m_selectedNodes.size() == 0) {
        showLayout(m_ui->nodeAttrs);
        hideLayout(m_ui->edgeAttrs);
    }

    m_ui->textMsg->hide();
    m_ui->inspectorContents->show();
    m_selectedNodes.insert(std::make_pair(node.id(), node));

    m_ui->ids->addItem(QString::number(node.id()));

    for (auto aw : m_nodeAttrWidgets) {
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
    }
}

void FullInspector::slotSelectedEdge(const Edge& edge)
{
    if (m_selectedNodes.size() > 0) {
        slotClear();
    } 
    
    if (m_selectedEdges.size() == 0) {
        showLayout(m_ui->edgeAttrs);
        hideLayout(m_ui->nodeAttrs);
    }
    m_ui->textMsg->hide();
    m_ui->inspectorContents->show();
    m_selectedEdges.insert(std::make_pair(edge.id(), edge));

    m_ui->ids->addItem(QString::number(edge.id()));

    for (auto aw : m_edgeAttrWidgets) {
        aw->blockSignals(true);
        aw->setValue(edge.attr(aw->id()));
        aw->blockSignals(false);
    }
}

void FullInspector::slotDeselectedNode(const Node& node)
{
    m_selectedNodes.erase(node.id());

    QListWidgetItem* lItem = m_ui->ids->findItems(QString::number(node.id()), Qt::MatchExactly).at(0);
    
    delete m_ui->ids->takeItem(m_ui->ids->row(lItem));
}

void FullInspector::slotDeselectedEdge(const Edge& edge)
{
    m_selectedEdges.erase(edge.id());

    QListWidgetItem* lItem = m_ui->ids->findItems(QString::number(edge.id()), Qt::MatchExactly).at(0);
    
    delete m_ui->ids->takeItem(m_ui->ids->row(lItem));
}

void FullInspector::slotHide()
{
    hide();
}

void FullInspector::slotShow()
{
    show();
}

}