/**
*  This file is part of Evoplex.
*
*  Evoplex is a multi-agent system for networks.
*  Copyright (C) 2019 - Eleftheria Chatziargyriou <ele.hatzy@gmail.com>
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

#ifndef GRAPHGENDLG_H
#define GRAPHGENDLG_H

#include <QComboBox>
#include <QFormLayout>
#include <QHash>
#include <QStringList>

#include "core/include/enum.h"
#include "core/mainapp.h"

#include "graphdesignerpage.h"
#include "maingui.h"

class Ui_GraphGenDlg;

namespace evoplex
{

class GraphDesignerPage;

class GraphGenDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GraphGenDlg(GraphDesignerPage* parent, MainGUI* mainGUI);
    ~GraphGenDlg();

private:
    Ui_GraphGenDlg * m_ui;
    GraphDesignerPage* m_graphPage;
    MainGUI* m_mainGUI;

    QHash<int, PluginKey> m_plugins; // Loaded plugins
    QHash<QString, AttrWidget*> m_attrWidgets; // Attribute widgets
    PluginKey m_selectedGraphKey;

    QStringList m_attrHeader;
    QStringList m_attrValues;
    int m_numNodes;
    GraphType m_graphType;

    QComboBox* m_cbgraphType;
    void parseAttrs(QString& error);

private slots:
    void slotGraphSelected(int grId);
    void slotSaveGraphGen();
};

} // evoplex
#endif // GRAPHGENDLG_H
