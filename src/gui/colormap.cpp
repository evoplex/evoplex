/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
        qWarning() << "[ColorMapMgr]: unable to open colormaps.json";
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
            Q_ASSERT(colors.size() > 0);
            CMapKey key(name, colors.size());
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
            qFatal("[ColorMapMgr]: unable to reset default settings!");
        }
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

    return new SingleColor(colors.front());
}

ColorMap::ColorMap(const Colors& colors)
    : m_colors(colors)
{
    Q_ASSERT(colors.size() > 0);
}

/************************************************************************/

SingleColor::SingleColor(QColor color)
    : ColorMap({color})
{
}

const QColor SingleColor::colorFromValue(const Value &val) const
{
    return m_colors.front();
}

/************************************************************************/

ColorMapRange::ColorMapRange(const Colors& colors, const RangeSpace* valSpace)
    : ColorMap(colors)
{
    if (valSpace->type() == ValueSpace::Int_Range) {
        m_max = valSpace->max().toInt();
        m_min = valSpace->min().toInt();
    } else if (valSpace->type() == ValueSpace::Double_Range) {
        m_max = valSpace->max().toDouble();
        m_min = valSpace->min().toDouble();
    } else {
        qFatal("[ColorMapRange] : invalid attribute space!");
    }
}

const QColor ColorMapRange::colorFromValue(const Value& val) const
{
    float value;
    if (val.type() == Value::INT) {
        value = val.toInt();
    } else if (val.type() == Value::DOUBLE) {
        value = val.toDouble();
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
        qWarning() << "[ColorMapSet]: invalid value!" << val.toQString();
        return Qt::black;
    }
    return i->second;
}

}
