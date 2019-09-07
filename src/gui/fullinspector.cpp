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

void FullInspector::slotChangeAttrScope(AttributesScope nodeAttrScope)
{
    while (m_ui->nodeAttrs->count()) {
        auto item = m_ui->nodeAttrs->takeRow(0);
        delete item.labelItem->widget();
        delete item.labelItem;
        delete item.fieldItem;
    }

    m_nodeAttrWidgets.clear();
    m_nodeAttrWidgets.resize(static_cast<size_t>(nodeAttrScope.size()));

    for (auto attrRange : nodeAttrScope) {
        auto aw = std::make_shared<AttrWidget>(attrRange, nullptr);
        aw->setToolTip(attrRange->attrRangeStr());
        int aId = aw->id();
        connect(aw.get(), &AttrWidget::valueChanged, [this, aId]() { attrValueChanged(aId); });
        m_nodeAttrWidgets.at(attrRange->id()) = aw;
        m_ui->nodeAttrs->insertRow(attrRange->id(), attrRange->attrName(), aw.get());

        QWidget* l = m_ui->nodeAttrs->labelForField(aw.get());
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
    }
}

void FullInspector::attrValueChanged(int attrId) const
{
    std::shared_ptr<AttrWidget> aw;
    try { aw = m_nodeAttrWidgets.at(attrId); }
    catch (std::out_of_range) { return; }
    
    for (auto node : m_selectedNodes) {
        Value v = aw->validate();
        if (v.isValid()) {
            node.second.setAttr(aw->id(), v);
        }
        else {
            aw->blockSignals(true);
            aw->setValue(node.second.attr(aw->id()));
            aw->blockSignals(false);
        }
    }
}

void FullInspector::slotSelectedNode(const Node& node)
{
    if (m_selectedNodes.size() == 0 &&  m_selectedEdges.size() == 0) {
        hideLayout(m_ui->edgeAttrs);
        showLayout(m_ui->nodeAttrs);
    } else if (m_selectedEdges.size() > 0) {
        slotClear();
        hideLayout(m_ui->edgeAttrs);
        showLayout(m_ui->nodeAttrs);
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
    if (m_selectedNodes.size() == 0 && m_selectedEdges.size() == 0) {
        hideLayout(m_ui->nodeAttrs);
        showLayout(m_ui->edgeAttrs);
    } else if (m_selectedNodes.size() > 0) {
        slotClear();
        showLayout(m_ui->nodeAttrs);
        hideLayout(m_ui->edgeAttrs);
    }
    m_ui->textMsg->hide();
    m_ui->inspectorContents->show();
    m_selectedEdges.insert(std::make_pair(edge.id(), edge));

    m_ui->ids->addItem(QString::number(edge.id()));
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