/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>

#include "ui_welcomewidget.h"

class WelcomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

private:
    Ui_WelcomeWidget* m_ui;
};

#endif // WELCOME_H
