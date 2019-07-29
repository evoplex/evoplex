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

#ifndef COLORMAP_H
#define COLORMAP_H

#include <QColor>
#include <QSettings>
#include <unordered_map>
#include <vector>

#include "attributes.h"
#include "attributerange.h"

namespace evoplex {

using Colors = std::vector<QColor>;
using CMapKey = QPair<QString, int>; // <name,size>

class ColorMapMgr
{
    friend class MainGUI;
public:
    void resetSettingsToDefault();

    const Colors colors(const CMapKey& key) const;
    const Colors colors(const QString& name, int size) const;
    inline QStringList names() const { return m_names; }
    inline QStringList sizes(const QString& name) const { return m_sizesAvailable.value(name); }

    inline const CMapKey& defaultCMapKey() const { return m_dfCMap; }
    inline void setDefaultColorMap(const CMapKey& key) { setDefaultColorMap(key.first, key.second); }
    void setDefaultColorMap(const QString& name, const int size);

private:
    explicit ColorMapMgr();

    QSettings m_userPrefs;
    CMapKey m_dfCMap;
    QHash<CMapKey, Colors> m_colormaps;
    QStringList m_names;
    QHash<QString, QStringList> m_sizesAvailable;
};

/************************************************************************/

class ColorMap
{
public:
    static ColorMap* create(AttributeRangePtr attrRange, const Colors& colors);

    virtual ~ColorMap();
    virtual const QColor& colorFromValue(const Value& val) const = 0;
    inline const AttributeRangePtr& attrRange() const { return m_attrRange; }
    inline const Colors& colors() const { return m_colors; }

protected:
    explicit ColorMap(AttributeRangePtr attrRange, const Colors& colors);
    AttributeRangePtr m_attrRange;
    Colors m_colors;
};

/************************************************************************/

class SingleColor : public ColorMap
{
public:
    explicit SingleColor(AttributeRangePtr attrRange, QColor color);
    virtual const QColor& colorFromValue(const Value& val) const;
};

/************************************************************************/

class ColorMapRange : public ColorMap
{
public:
    explicit ColorMapRange(AttributeRangePtr attrRange, const Colors& colors);
    virtual const QColor& colorFromValue(const Value& val) const;

private:
    float m_max;
    float m_min;
};

/************************************************************************/

class ColorMapSet : public ColorMap
{
public:
    explicit ColorMapSet(AttributeRangePtr attrRange, const Colors& colors);
    virtual const QColor& colorFromValue(const Value& val) const;

private:
    std::unordered_map<Value, QColor> m_cmap;
};

}

#endif // COLORMAP_H
