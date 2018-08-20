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

#ifndef ATTRCOLORSELECTOR_H
#define ATTRCOLORSELECTOR_H

#include <QWidget>

#include "core/include/attributerange.h"
#include "colormap.h"

class Ui_AttrColorSelector;

namespace evoplex {
class AttrColorSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AttrColorSelector(QWidget *parent = nullptr);
    ~AttrColorSelector();

    void init(ColorMapMgr* cmapMgr, CMapKey cmap, AttributesScope scope);
    QString cmapName() const;
    int cmapSize() const;
    inline ColorMap* cmap() const;

signals:
    void cmapUpdated(ColorMap* cmap);

private slots:
    void updateCMap();
    void slotCMapName(const QString& name);

private:
    Ui_AttrColorSelector* m_ui;
    ColorMapMgr* m_cmapMgr;
    AttributesScope m_attrScope;
    ColorMap* m_cmap;

    void blockAllSignals(bool b);
};

inline ColorMap* AttrColorSelector::cmap() const
{ return m_cmap; }

} // evoplex
#endif // ATTRCOLORSELECTOR_H
