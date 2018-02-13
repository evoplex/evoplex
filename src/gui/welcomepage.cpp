/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

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
