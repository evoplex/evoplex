/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>

#include "colormap.h"

namespace evoplex
{

ColorMapMgr::ColorMapMgr()
    : m_defaultColorMap("black")
{
    CMap df;
    df.name = m_defaultColorMap;
    df.colors.insert(1, {Qt::black});
    m_colormaps.insert(m_defaultColorMap, df);

    QFile file(":colormaps/colormaps.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ColorMapMgr]: unable to open colormaps.json";
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
    for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
        CMap cmap;
        cmap.name = it.key();
        QJsonObject objColor = it.value().toObject();
        for (QJsonObject::iterator it2 = objColor.begin(); it2 != objColor.end(); ++it2) {
            Colors colors;
            for (QJsonValueRef rgb_ : it2.value().toArray()) {
                QJsonArray rgb = rgb_.toArray();
                colors.emplace_back(QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt()));
            }
            cmap.colors.insert(colors.size(), colors);
        }
        m_colormaps.insert(cmap.name, cmap);
    }
}

/************************************************************************/

ColorMap* ColorMap::create(const ValueSpace* valSpace, const Colors& colors)
{
    const SetSpace* set = dynamic_cast<const SetSpace*>(valSpace);
    if (set) {
        return new ColorMapSet(colors, set);
    }

    const RangeSpace* range = dynamic_cast<const RangeSpace*>(valSpace);
    if (range) {
        return new ColorMapRange(colors, range);
    }

    qFatal("[ColorMap] : invalid attribute space for %s", valSpace->attrName());
}

ColorMap::ColorMap(const Colors& colors)
    : m_colors(colors)
{
    Q_ASSERT(colors.size() > 0);
}

/************************************************************************/

ColorMapRange::ColorMapRange(const Colors& colors, const RangeSpace* valSpace)
    : ColorMap(colors)
{
    if (valSpace->type() == ValueSpace::Int_Range) {
        m_max = valSpace->max().toInt;
        m_min = valSpace->min().toInt;
    } else if (valSpace->type() == ValueSpace::Double_Range) {
        m_max = valSpace->max().toDouble;
        m_min = valSpace->min().toDouble;
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
        qFatal("[ColorMapRange]: invalid attribute space!");
    }
    return m_colors.at(std::round((value * (m_colors.size()-1)) / m_max + m_min));
}

/************************************************************************/

ColorMapSet::ColorMapSet(const Colors& colors, const SetSpace* valSpace)
    : ColorMap(colors)
{
    int c = 0;
    for (const Value value : valSpace->values()) {
        m_cmap.insert({value, m_colors.at(c++)});
        c = (c == m_colors.size()) ? 0 : c;
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
