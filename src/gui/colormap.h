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
#include "core/valuespace.h"

namespace evoplex
{

typedef std::vector<QColor> Colors;

class ColorMapMgr
{
public:
    struct CMap {
        QString name;
        QHash<int, Colors> colors; // colors.size as key
    };

    explicit ColorMapMgr();

    inline QStringList names() const { return m_colormaps.keys(); }
    inline const QString& defaultColorMap() const { return m_defaultColorMap; }
    inline void setDefaultColorMap(const QString& cm) { m_defaultColorMap = cm; }

private:
    QHash<QString, CMap> m_colormaps;
    QString m_defaultColorMap;
};

class ColorMap
{
public:/*
    enum CMap {
        DivergingSet1,
        Blues
    };

    QColor divergingSet1[4] = {
        QColor(43,131,186),  // blue
        QColor(215,25,28),   // red
        QColor(171,221,164), // green
        QColor(253,174,97)   // orange
    };

    QColor blues[4] = {
        QColor(239,243,255),
        QColor(189,215,231),
        QColor(107,174,214),
        QColor(33,113,181)
    };
    */

    static ColorMap* create(const ValueSpace* valSpace, const Colors& colors);

    virtual const QColor colorFromValue(const Value& val) const = 0;

protected:
    explicit ColorMap(const Colors& colors);
    const Colors& m_colors;
};

class ColorMapRange : public ColorMap
{
public:
    explicit ColorMapRange(const Colors& colors, const RangeSpace* valSpace);
    virtual const QColor colorFromValue(const Value& val) const;
private:
    float m_max;
    float m_min;
};

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
