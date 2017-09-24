/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

#include "core/mainapp.h"

class Ui_SettingsWidget;

namespace evoplex {

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(MainApp* mainApp, QWidget* parent = 0);
    ~SettingsWidget();

private slots:
    void slotNumThreads(int newValue);

private:
    Ui_SettingsWidget* m_ui;
    MainApp* m_mainApp;
};
}
#endif // SETTINGSWIDGET_H
