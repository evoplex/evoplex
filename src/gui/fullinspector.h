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

#ifndef FULL_INSPECTOR_H
#define FULL_INSPECTOR_H

#include <QDockWidget>

#include "core/include/attributerange.h"
#include "core/include/node.h"

#include "attrwidget.h"

class Ui_FullInspector;

namespace evoplex {

class FullInspector : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit FullInspector(QWidget* parent);
    ~FullInspector();
   
public slots:
    void slotClear();
    void slotHide();
    void slotShow();
    void slotSelectedNode(Node& node);
    void slotChangeAttrScope(AttributesScope nodeAttrScope);


private:
    QWidget* m_parent;
    Ui_FullInspector* m_ui;
    std::vector<Node> m_selectedNodes;
    std::vector<std::shared_ptr<AttrWidget>> m_attrWidgets;

    void attrValueChanged(int attrId) const;
};

}

#endif // FULL_INSPECTOR_H