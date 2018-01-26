/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <cmath>

#include "colormap.h"
#include "core/utils.h"

namespace evoplex {

ColorMap::ColorMap(CMap cmap, const AttributesSpace& attrsSpace)
    : m_attrsSpace(attrsSpace)
    , m_mapValue(nullptr)
{
    switch (cmap) {
    case DivergingSet1:
        m_colors = {
            QColor(43,131,186),  // blue
            QColor(215,25,28),   // red
            QColor(171,221,164), // green
            QColor(253,174,97)   // orange
        };
        break;
    case Blues:
        m_colors = {
            QColor(239,243,255),
            QColor(189,215,231),
            QColor(107,174,214),
            QColor(33,113,181),
        };
        break;
    default:
        qFatal("[ColorMap]: invalid colormap!");
    }
    m_colors.shrink_to_fit();
}

ColorMap::~ColorMap()
{
    delete m_mapValue;
}

void ColorMap::setAttr(const QString& attrName)
{
    if (m_mapValue) {
        delete m_mapValue;
    }
    const ValueSpace* valSpace = m_attrsSpace.value(attrName);
    const SetSpace* set = dynamic_cast<const SetSpace*>(valSpace);
    if (set) {
        m_mapValue = new ColorMapSet(m_colors, set->values());
    } else if (dynamic_cast<const RangeSpace*>(valSpace)) {
        m_mapValue = new ColorMapRange(m_colors, valSpace->min(), valSpace->max());
    } else {
        qFatal("[ColorMap] : invalid attribute space for %s", attrName);
    }
}

/************************************************************************/

ColorMapRange::ColorMapRange(const std::vector<QColor>& colors, const Value& min, const Value& max)
    : m_colors(colors)
{
    Q_ASSERT(colors.size() > 0);

    if (min.type == Value::INT) {
        m_max = max.toInt;
        m_min = min.toInt;
    } else if (min.type == Value::DOUBLE) {
        m_max = max.toDouble;
        m_min = min.toDouble;
    } else {
        qFatal("[ColorMapRange] : invalid attribute space!");
    }
}

const QColor ColorMapRange::colorFromValue(const Value& val) const
{
    float value;
    if (val.type == Value::INT) {
        value = val.toInt;
    } else if (val.type == Value::DOUBLE) {
        value = val.toDouble;
    } else {
        qFatal("[ColorMapRange] : invalid attribute space!");
    }
    return m_colors.at(std::round((value * (m_colors.size()-1)) / m_max + m_min));
}

/************************************************************************/

ColorMapSet::ColorMapSet(const std::vector<QColor>& colors, const Values &vals)
{
    Q_ASSERT(colors.size() > 0);

    int c = 0;
    for (int i = 0; i < vals.size(); ++i) {
        m_cmap.insert({vals.at(i), colors.at(c++)});
        c = (c == colors.size()) ? 0 : c;
    }
}

const QColor ColorMapSet::colorFromValue(const Value& val) const
{
    std::unordered_map<Value, QColor>::const_iterator i = m_cmap.find(val);
    if (i == m_cmap.end()) {
        qFatal("[ColorMapSet]: invalid value! (%s)", val.toQString());
    }
    return i->second;
}

}
