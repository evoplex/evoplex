/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

#include "maingui.h"
#include "core/mainapp.h"

class Ui_SettingsPage;

namespace evoplex {

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(MainGUI* mainGUI);
    ~SettingsPage();

    void refreshFields();

private slots:
    void setDfCMapName(const QString& name);
    void setDfCMapSize(const QString& sz);
    void resetDefaults();

private:
    Ui_SettingsPage* m_ui;
    MainGUI* m_mainGUI;
};
}
#endif // SETTINGSPAGE_H
