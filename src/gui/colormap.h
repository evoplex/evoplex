/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef COLORMAP_H
#define COLORMAP_H

#include <QColor>
#include <unordered_map>
#include <vector>

#include "core/attributes.h"

namespace evoplex {

class ColorMapValue
{
public:
    virtual const QColor colorFromValue(const Value& val) const = 0;
};

class ColorMapInterval : public ColorMapValue
{
public:
    ColorMapInterval(const std::vector<QColor>& colors, const Value& min, const Value& max);

    const QColor colorFromValue(const Value& val) const;

private:
    const std::vector<QColor> m_colors;
    float m_max;
    float m_min;
};

class ColorMapSet : public ColorMapValue
{
public:
    ColorMapSet(const std::vector<QColor>& colors, const QVector<Value>& vals);

    const QColor colorFromValue(const Value& val) const;

private:
    std::unordered_map<Value, QColor> m_cmap;
};

class ColorMap
{
public:
    enum CMap {
        DivergingSet1,
        Blues
    };

    ColorMap(CMap cmap, const AttributesSpace& attrsSpace);
    ~ColorMap();

    void setAttr(const QString& attrName);

    inline const QColor colorFromValue(Value val) const
    { return m_mapValue->colorFromValue(val); }

private:
    const AttributesSpace& m_attrsSpace;
    ColorMapValue* m_mapValue;
    std::vector<QColor> m_colors;
};

}

#endif // COLORMAP_H
