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
}

FullInspector::~FullInspector() 
{
    delete m_ui;
    m_attrWidgets.clear();
}

void FullInspector::slotClear() {
    m_ui->ids->clear();

    m_ui->textMsg->show();
    m_ui->inspectorContents->hide();
}

void FullInspector::slotChangeAttrScope(AttributesScope nodeAttrScope)
{
    while (m_ui->attrs->count()) {
        auto item = m_ui->attrs->takeRow(0);
        delete item.labelItem->widget();
        delete item.labelItem;
        delete item.fieldItem;
    }

    m_attrWidgets.clear();
    m_attrWidgets.resize(static_cast<size_t>(nodeAttrScope.size()));

    for (auto attrRange : nodeAttrScope) {
        auto aw = std::make_shared<AttrWidget>(attrRange, nullptr);
        aw->setToolTip(attrRange->attrRangeStr());
        int aId = aw->id();
        connect(aw.get(), &AttrWidget::valueChanged, [this, aId]() { attrValueChanged(aId); });
        m_attrWidgets.at(attrRange->id()) = aw;
        m_ui->attrs->insertRow(attrRange->id(), attrRange->attrName(), aw.get());

        QWidget* l = m_ui->attrs->labelForField(aw.get());
        l->setToolTip(attrRange->attrName());
        l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        l->setMinimumWidth(m_ui->lattrs->minimumWidth());
    }

}

void FullInspector::attrValueChanged(int attrId) const
{
    //TODO
}

void FullInspector::slotSelectedNode(const Node& node)
{
    m_ui->textMsg->hide();
    m_ui->inspectorContents->show();

    m_ui->ids->addItem(QString::number(node.id()));

    for (auto aw : m_attrWidgets) {
        aw->blockSignals(true);
        aw->setValue(node.attr(aw->id()));
        aw->blockSignals(false);
    }

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