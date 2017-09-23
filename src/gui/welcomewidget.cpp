/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "gui/maingui.h"
#include "gui/welcomewidget.h"

namespace evoplex {

WelcomeWidget::WelcomeWidget(MainGUI* maingui)
    : QWidget(maingui)
    , m_ui(new Ui_WelcomeWidget)
    , m_maingui(maingui)
{
    m_ui->setupUi(this);

    connect(m_ui->bNewProject, SIGNAL(pressed()), m_maingui, SLOT(newProject()));
}

WelcomeWidget::~WelcomeWidget()
{
    delete m_ui;
    m_ui = nullptr;
}
}
