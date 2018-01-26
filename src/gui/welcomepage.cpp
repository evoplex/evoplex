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
    connect(m_ui->bOpenProject, SIGNAL(pressed()), m_maingui, SIGNAL(openProject()));
}

WelcomePage::~WelcomePage()
{
    delete m_ui;
    m_ui = nullptr;
}
}
