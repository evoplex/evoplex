/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

#include "core/mainapp.h"

class Ui_SettingsPage;

namespace evoplex {

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(MainApp* mainApp, QWidget* parent = 0);
    ~SettingsPage();

private slots:
    void slotNumThreads(int newValue);

private:
    Ui_SettingsPage* m_ui;
    MainApp* m_mainApp;
};
}
#endif // SETTINGSPAGE_H
