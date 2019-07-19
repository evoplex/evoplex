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
#include <QMessageBox>

#include "graphgendlg.h"
#include "ui_graphgendlg.h"
#include "maingui.h"

namespace evoplex {

GraphGenDlg::GraphGenDlg(GraphDesignerPage* parent)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
    m_ui(new Ui_GraphGenDlg),
    m_graphPage(parent)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    m_ui->setupUi(this);
    setVisible(true);

    //connect(m_ui->ok, SIGNAL(clicked()), SLOT(slotGraphSaved()));
    connect(m_ui->cancel, SIGNAL(clicked()), SLOT(close()));
};

GraphGenDlg::~GraphGenDlg()
{
    delete m_ui;
}

}