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
#include <QSet>
#include <QTableWidgetItem>
#include <QHash>
#include <QMessageBox>

#include "graphattrsdlg.h"
#include "ui_graphattrsdlg.h"
#include "maingui.h"

namespace evoplex {

GraphAttrsDlg::GraphAttrsDlg(GraphDesignerPage* parent, const AttrsType type)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
    m_ui(new Ui_GraphAttrsDlg),
    m_type(type),
    m_graphPage(parent)
{
    setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);
    setVisible(true);

    connect(m_ui->numAttrs, SIGNAL(valueChanged(int)), SLOT(slotTableUpdate(int)));
    connect(m_ui->ok, SIGNAL(clicked()), SLOT(slotAttrSaved()));
    connect(m_ui->cancel, SIGNAL(clicked()), SLOT(close()));

    if ((m_type == AttrsType::Nodes && !m_graphPage->nodeAttributesScope().isEmpty()) ||
        (m_type == AttrsType::Edges && !m_graphPage->edgeAttributesScope().isEmpty())) {
        displayCurrentAttrs();
    }
};

GraphAttrsDlg::~GraphAttrsDlg()
{
    delete m_ui;
}

void GraphAttrsDlg::displayCurrentAttrs()
{
    AttributesScope attrsScope;
    if (m_type == AttrsType::Nodes) {
        attrsScope = m_graphPage->nodeAttributesScope();
    } else {
        attrsScope = m_graphPage->edgeAttributesScope();
    }

    m_ui->numAttrs->setValue(attrsScope.size());
    int i = 0;
    
    for (auto attrRange : attrsScope) {    
        m_ui->table->setItem(i, 0, new QTableWidgetItem(attrRange->attrName()));
        m_ui->table->setItem(i, 1, new QTableWidgetItem(attrRange->attrRangeStr()));
        ++i;
    }
}

void GraphAttrsDlg::parseAttributes(QString& error)
{
    QString _name;
    QString _attrRange;
    QSet<QString> failedAttrs;
    QSet<QString> curAttrs;

    AttributesScope attrsScope;
    attrsScope.reserve(m_ui->table->rowCount());

    for (int i = 0; i < m_ui->table->rowCount(); ++i) {
        if (!m_ui->table->item(i, 0)|| !m_ui->table->item(i, 1)) {
            error = "Empty field is not allowed";
            return;
        }
        _name = m_ui->table->item(i, 0)->text();
        _attrRange = m_ui->table->item(i, 1)->text();

        auto attrs = AttributeRange::parse(i, _name, _attrRange);
        if (!attrs.get()->isValid()) {
            failedAttrs.insert(_name);
            curAttrs.insert(_name);
        } else if (curAttrs.contains(_name)) {
            error = "Duplicate attributes are not allowed";
            return;
        } else {
            attrsScope.insert(attrs->attrName(), attrs);
            curAttrs.insert(_name);
        }
    }

    if (!failedAttrs.isEmpty()) {
        error = "Unable to parse attributes: \n";
        for (QSet<QString>::iterator i = failedAttrs.begin(); i != failedAttrs.end(); ++i) {
            error += *i + ' ';
        }
        return;
    }
    m_attrsScope = attrsScope;
}

void GraphAttrsDlg::slotTableUpdate(const int n)
{
    m_ui->table->setRowCount(n);
}

void GraphAttrsDlg::slotAttrSaved() {
    QString errstrng;
    parseAttributes(errstrng);

    if (!errstrng.isEmpty()) {
        QMessageBox::warning(this, "Attributes Parser", "Error when parsing attributes: " + errstrng);
        return;
    }

    m_graphPage->changedAttrsScope(m_type, m_attrsScope);
    close();
}

}