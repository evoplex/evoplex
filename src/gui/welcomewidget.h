/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>

#include "ui_welcomewidget.h"

namespace evoplex {

class MainGUI;

class WelcomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(MainGUI* maingui);
    ~WelcomeWidget();

private:
    Ui_WelcomeWidget* m_ui;
    MainGUI* m_maingui;
};
}

#endif // WELCOME_H
