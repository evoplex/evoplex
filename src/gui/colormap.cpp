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

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <cmath>

#include "colormap.h"

namespace evoplex
{

ColorMapMgr::ColorMapMgr()
{
    m_dfCMap = CMapKey("Black", 1);
    m_colormaps.insert(m_dfCMap, {Qt::black});
    m_names.append(m_dfCMap.first);
    m_sizesAvailable.insert(m_dfCMap.first, {"1"});

    QFile file(":colormaps/colormaps.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "unable to open colormaps.json";
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
    for (QJsonObject::iterator it = json.begin(); it != json.end(); ++it) {
        QString name = it.key();
        QStringList sizes;
        for (QJsonValueRef setOfColors : it.value().toObject()) {
            if (!setOfColors.isArray()) {
                continue;
            }
            Colors colors;
            for (QJsonValueRef rgb_ : setOfColors.toArray()) {
                QJsonArray rgb = rgb_.toArray();
                colors.emplace_back(QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt()));
            }
            Q_ASSERT_X(colors.size() > 0 && colors.size() < INT_MAX, "ColorMapMgr", "the color size is invalid!");
            CMapKey key(name, static_cast<int>(colors.size()));
            m_colormaps.insert(key, colors);
            sizes.append(QString::number(key.second));
        }
        m_names.append(name);
        qSort(sizes.begin(), sizes.end(), [](const QString& s1, const QString& s2){ return s1.toInt() < s2.toInt(); });
        m_sizesAvailable.insert(name, sizes);
    }
    m_names.sort();

    CMapKey userDfCMap;
    userDfCMap.first = m_userPrefs.value("settings/colormap", "Evoplex").toString();
    userDfCMap.second = m_userPrefs.value("settings/colormapSize", 5).toInt();
    m_dfCMap = m_colormaps.contains(userDfCMap) ? userDfCMap : m_dfCMap;
}

const Colors ColorMapMgr::colors(const CMapKey& key) const
{
    const Colors colors = m_colormaps.value(key, Colors());
    if (colors.empty()) {
        return m_colormaps.value(m_dfCMap);
    }
    return colors;
}

const Colors ColorMapMgr::colors(const QString& name, int size) const
{
    return colors(CMapKey(name, size));
}

void ColorMapMgr::setDefaultColorMap(const QString& name, const int size)
{
    m_dfCMap = CMapKey(name, size);
    m_userPrefs.setValue("settings/colormap", name);
    m_userPrefs.setValue("settings/colormapSize", size);
}

void ColorMapMgr::resetSettingsToDefault()
{
    m_dfCMap = qMakePair(QString("Evoplex"), 5);
    if (!m_colormaps.contains(m_dfCMap)) {
        m_dfCMap = qMakePair(QString("Black"), 1);
        if (!m_colormaps.contains(m_dfCMap)) {
            qFatal("unable to reset default settings!");
        }
    }
}

/************************************************************************/

ColorMap* ColorMap::create(AttributeRangePtr attrRange, const Colors& colors)
{
    auto set = dynamic_cast<SetOfValues*>(attrRange.get());
    if (set) {
        return new ColorMapSet(colors, set);
    }

    auto interval = dynamic_cast<IntervalOfValues*>(attrRange.get());
    if (interval) {
        return new ColorMapRange(colors, interval);
    }

    return new SingleColor(colors.front());
}

ColorMap::ColorMap(const Colors& colors)
    : m_colors(colors)
{
    Q_ASSERT_X(colors.size() > 0, "ColorMap", "the color size is invalid!");
}

ColorMap::~ColorMap()
{
}

/************************************************************************/

SingleColor::SingleColor(QColor color)
    : ColorMap({color})
{
}

const QColor SingleColor::colorFromValue(const Value& val) const
{
    Q_UNUSED(val);
    return m_colors.front();
}

/************************************************************************/

ColorMapRange::ColorMapRange(const Colors& colors, const IntervalOfValues* attrRange)
    : ColorMap(colors)
{
    if (attrRange->type() == AttributeRange::Int_Range) {
        m_max = attrRange->max().toInt();
        m_min = attrRange->min().toInt();
    } else if (attrRange->type() == AttributeRange::Double_Range) {
        m_max = static_cast<float>(attrRange->max().toDouble());
        m_min = static_cast<float>(attrRange->min().toDouble());
    } else if (attrRange->type() == AttributeRange::Bool) {
        m_max = 1.f;
        m_min = 0.f;
    } else {
        qFatal("invalid attribute range!");
    }
}

const QColor ColorMapRange::colorFromValue(const Value& val) const
{
    float value;
    if (val.type() == Value::INT) {
        value = val.toInt();
    } else if (val.type() == Value::DOUBLE) {
        value = static_cast<float>(val.toDouble());
    } else if (val.type() == Value::BOOL) {
        return m_colors.at(val.toBool());
    } else {
        qFatal("invalid attribute range!");
    }
    return m_colors.at(static_cast<size_t>(std::round((value * (m_colors.size()-1)) / m_max) + m_min));
}

/************************************************************************/

ColorMapSet::ColorMapSet(const Colors& colors, const SetOfValues* attrRange)
    : ColorMap(colors)
{
    size_t c = 0;
    for (const Value& value : attrRange->values()) {
        m_cmap.insert({value, m_colors.at(c++)});
        c = (c == m_colors.size()) ? 0 : c;
    }
}

const QColor ColorMapSet::colorFromValue(const Value& val) const
{
    const auto i = m_cmap.find(val);
    if (i == m_cmap.cend()) {
        // qWarning() << "invalid value!" << val.toQString();
        return Qt::black;
    }
    return i->second;
}

}
