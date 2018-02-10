/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QSettings>
#include <QWidget>

class Ui_WelcomePage;

namespace evoplex {

class MainGUI;

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomePage(MainGUI* maingui);
    ~WelcomePage();

private slots:
    void refreshList();

private:
    QSettings m_userPrefs;
    Ui_WelcomePage* m_ui;
    MainGUI* m_maingui;
};
}

#endif // WELCOMEPAGE_H
