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

#include "core/experiment.h"
#include "attrcolorselector.h"
#include "colormap.h"

class Ui_GraphSettings;

namespace evoplex {

class GraphView;

class GraphSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GraphSettings(ColorMapMgr* cMgr, ExperimentPtr exp, GraphView* parent);
    ~GraphSettings();

    int nodeScale() const;
    int edgeScale() const;
    int edgeWidth() const;

    AttrColorSelector* nodeColorSelector() const;
    AttrColorSelector* edgeColorSelector() const;

public slots:
    void init();
    void restoreSettings();
    void saveAsDefault();

private slots:
    void slotNodeScale(int v);
    void slotEdgeScale(int v);
    void slotEdgeWidth(int v);

private:
    Ui_GraphSettings* m_ui;
    GraphView* m_parent;
    ColorMapMgr* m_cMgr;
    const ExperimentPtr m_exp;
};

} // evoplex
#endif // GRAPHSETTINGS_H
