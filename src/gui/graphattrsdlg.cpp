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

#include <QDebug>
#include <QMessageBox>

#include "graphattrsdlg.h"
#include "ui_graphattrsdlg.h"
#include "maingui.h"

namespace evoplex {

GraphAttrsDlg::GraphAttrsDlg(QWidget* parent)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
    m_ui(new Ui_GraphAttrsDlg)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    m_ui->setupUi(this);
    setVisible(true);

    connect(m_ui->numAttrs, SIGNAL(valueChanged(int)), SLOT(slotTableUpdate(int)));
};

GraphAttrsDlg::~GraphAttrsDlg()
{
}

void GraphAttrsDlg::slotTableUpdate(int n)
{
    m_ui->table->setRowCount(n);
}

}