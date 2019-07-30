/**
*  This file is part of Evoplex.
*
*  Evoplex is a multi-agent system for networks.
*  Copyright (C) 2019 - Eleftheria Chatziargyriou
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

#ifndef GRAPHDESIGNERPAGE_H
#define GRAPHDESIGNERPAGE_H

#include <QMainWindow>
#include <QStringList>

#include "core/include/enum.h"

#include "maingui.h"
#include "abstractgraph.h"
#include "attributerange.h"

#include "core/mainapp.h"
#include "core/graphinputs.h"

class Ui_GraphDesignerPage;

namespace evoplex {

class GraphDesigner;
enum class  AttrsType;

class GraphDesignerPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphDesignerPage(MainGUI* mainGUI);
    ~GraphDesignerPage();

protected:
    friend class GraphAttrsDlg;
    friend class GraphGenDlg;
    friend class GraphDesigner;

    void changedAttrsScope(const AttrsType type, AttributesScope attrs);
    void changedGraphAttrs(const int numNodes, PluginKey selectedGraphKey, GraphType graphType, QStringList& graphAttrHeader, 
        QStringList& graphAttrValues, QString& error);

    inline AttributesScope edgeAttributesScope() const;
    inline AttributesScope nodeAttributesScope() const;
    inline QStringList graphAttrHeader() const;
    inline QStringList graphAttrValues() const;
    inline PluginKey selectedGraphKey() const;

private:
    MainApp* m_mainApp;
    MainGUI* m_mainGUI;
    QMainWindow* m_innerWindow;
    Ui_GraphDesignerPage * m_ui;
    GraphDesigner* m_graphDesigner;

    PluginKey m_selectedGraphKey;
    AttributesScope m_edgeAttrScope;
    AttributesScope m_nodeAttrScope;
    int m_numNodes;
    GraphType m_graphType;
    QStringList m_graphAttrHeader;
    QStringList m_graphAttrValues;

    GraphInputsPtr parseInputs();

private slots:
    void slotEdgeAttrs();
    void slotNodeAttrs();
    void slotGraphGen();

signals:
    void openSettingsDlg();
};

inline AttributesScope GraphDesignerPage::edgeAttributesScope() const
{
    return m_edgeAttrScope;
}

inline AttributesScope GraphDesignerPage::nodeAttributesScope() const
{
    return m_nodeAttrScope;
}

inline QStringList GraphDesignerPage::graphAttrHeader() const
{
    return m_graphAttrHeader;
}

inline QStringList GraphDesignerPage::graphAttrValues() const
{
    return m_graphAttrValues;
}

inline PluginKey GraphDesignerPage::selectedGraphKey() const
{
    return m_selectedGraphKey;
}

}

#endif // GRAPHDESIGNERPAGE_H
