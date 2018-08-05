/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLUGINSPAGE_H
#define PLUGINSPAGE_H

#include <QWidget>

#include "maingui.h"
#include "core/mainapp.h"

class Ui_PluginsPage;

namespace evoplex {
class PluginsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsPage(MainGUI* mainGUI);
    ~PluginsPage();

private slots:
    void importPlugin();
    void rowSelectionChanged();
    void insertRow(const Plugin* plugin);

private:
    enum TableCols {
        TYPE = 0,
        UID = 1,
        VERSION = 2,
        NAME = 3,
        UNLOAD = 4
    };

    Ui_PluginsPage* m_ui;
    MainApp* m_mainApp;
    QMainWindow* m_innerWindow;

    void loadHtml(const Plugin* plugin);
};

class ButtonHoverWatcher : public QObject
{
    Q_OBJECT
public:
    explicit ButtonHoverWatcher(QObject* parent) : QObject(parent) {}
    virtual bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
};

}
#endif // PLUGINSPAGE_H
