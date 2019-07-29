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

#ifndef GRIDSETTINGS_H
#define GRIDSETTINGS_H

#include <QDialog>

#include "attrcolorselector.h"
#include "maingui.h"

class Ui_GridSettings;

namespace evoplex {

class GridSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GridSettings(ColorMapMgr* cMgr, QWidget *parent);
    ~GridSettings();

    void setup(AttributesScope nodeAttrsScope);

    AttrColorSelector* nodeColorSelector() const;

public slots:
    void restoreSettings();
    void saveAsDefault();

private:
    Ui_GridSettings* m_ui;
    ColorMapMgr* m_cMgr;
    AttributesScope m_nodeAttrsScope;
};

} // evoplex
#endif // GRIDSETTINGS_H
