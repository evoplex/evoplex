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

#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QDialog>

#include "maingui.h"
#include "core/experiment.h"

class Ui_GraphSettings;

namespace evoplex
{

class GraphSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GraphSettings(MainGUI* mainGUI, Experiment* exp, QWidget* parent);
    ~GraphSettings();

    int nodeAttr() const;
    int edgeAttr() const;
    inline ColorMap* nodeCMap() const { return m_nodeCMap; }
    inline ColorMap* edgeCMap() const { return m_edgeCMap; }

signals:
    void nodeAttrUpdated(int);
    void edgeAttrUpdated(int);
    void nodeCMapUpdated(ColorMap* cmap);
    void edgeCMapUpdated(ColorMap* cmap);

private slots:
    void init();

    void updateNodeCMap();
    void slotNodeCMapName(const QString& name);
    void setNodeAttr(int attrIdx);

    void updateEdgeCMap();
    void slotEdgeCMapName(const QString& name);
    void setEdgeAttr(int attrIdx);

private:
    Ui_GraphSettings* m_ui;
    const Experiment* m_exp;
    const ModelPlugin* m_modelPlugin;
    const ColorMapMgr* m_cmMgr;
    ColorMap* m_nodeCMap;
    ColorMap* m_edgeCMap;
};

} // evoplex
#endif // GRAPHSETTINGS_H
