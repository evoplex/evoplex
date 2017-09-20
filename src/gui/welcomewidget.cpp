/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "gui/welcomewidget.h"

WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_WelcomeWidget)
{
    m_ui->setupUi(this);
}

WelcomeWidget::~WelcomeWidget()
{
    delete m_ui;
    m_ui = nullptr;
}
