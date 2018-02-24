/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef COLORMAP_H
#define COLORMAP_H

#include <QColor>
#include <QSettings>
#include <unordered_map>
#include <vector>

#include "attributes.h"
#include "valuespace.h"

namespace evoplex
{

typedef std::vector<QColor> Colors;
typedef QPair<QString, int> CMapKey; // <name,size>

class ColorMapMgr
{
public:
    explicit ColorMapMgr();

    void resetSettingsToDefault();

    const Colors colors(const CMapKey& key) const;
    const Colors colors(const QString& name, int size) const;
    inline QStringList names() const { return m_names; }
    inline QStringList sizes(const QString& name) const { return m_sizesAvailable.value(name); }

    inline const CMapKey& defaultColorMap() const { return m_dfCMap; }
    inline void setDefaultColorMap(const CMapKey& key) { setDefaultColorMap(key.first, key.second); }
    void setDefaultColorMap(const QString& name, const int size);

private:
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
    static ColorMap* create(const ValueSpace* valSpace, const Colors& colors);

    virtual const QColor colorFromValue(const Value& val) const = 0;

protected:
    explicit ColorMap(const Colors& colors);
    const Colors m_colors;
};

/************************************************************************/

class SingleColor : public ColorMap
{
public:
    explicit SingleColor(QColor color);
    virtual const QColor colorFromValue(const Value& val) const;
};

/************************************************************************/

class ColorMapRange : public ColorMap
{
public:
    explicit ColorMapRange(const Colors& colors, const RangeSpace* valSpace);
    virtual const QColor colorFromValue(const Value& val) const;
private:
    float m_max;
    float m_min;
};

/************************************************************************/

class ColorMapSet : public ColorMap
{
public:
    explicit ColorMapSet(const Colors& colors, const SetSpace* valSpace);
    virtual const QColor colorFromValue(const Value& val) const;
private:
    std::unordered_map<Value, QColor> m_cmap;
};

}

#endif // COLORMAP_H
