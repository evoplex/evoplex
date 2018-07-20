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

#include "fontstyles.h"
#include "maingui.h"
#include "welcomepage.h"
#include "ui_welcomepage.h"

namespace evoplex {

WelcomePage::WelcomePage(MainGUI* maingui)
    : QWidget(maingui)
    , m_ui(new Ui_WelcomePage)
    , m_maingui(maingui)
{
    m_ui->setupUi(this);

    m_ui->labelEvoplex->setFont(FontStyles::h4());
    m_ui->labelStart->setFont(FontStyles::h6());
    m_ui->labelRecent->setFont(FontStyles::h6());

    connect(m_ui->bNewProject, SIGNAL(pressed()), m_maingui, SIGNAL(newProject()));
    connect(m_ui->bOpenProject, &QPushButton::pressed, [this]() { m_maingui->openProject(""); });

    refreshList();
    connect(maingui->mainApp(), SIGNAL(listOfRecentProjectsUpdated()), SLOT(refreshList()));
    connect(m_ui->recent, &QListWidget::doubleClicked, [this](const QModelIndex& index) {
        emit (m_maingui->openProject(index.data().toString()));
    });
}

WelcomePage::~WelcomePage()
{
    delete m_ui;
}

void WelcomePage::refreshList()
{
    m_ui->recent->clear();
    QVariantList recentProjects = m_userPrefs.value("recentProjects").toList();
    for (const QVariant& path : recentProjects) {
        m_ui->recent->addItem(path.toString());
    }
}

} // evoplex
