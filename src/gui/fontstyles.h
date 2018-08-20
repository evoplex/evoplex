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

#ifndef FONTSTYLES_H
#define FONTSTYLES_H

#include <QFont>

/**
 * @brief Convenient class to apply font styles.
 * Based on: https://material.io/design/typography/the-type-system.html#type-scale
 */
class FontStyles
{
public:
    // do not rely on Qt bold() and italic() functions
    static const QString regular;
    static const QString bold;
    static const QString italic;
    static const QString light;
    static const QString medium;

    static QFont font(const QString& name, int pixelSize, qreal letterSpacing);
    static QFont h4();
    static QFont h5();
    static QFont h6();
    static QFont subtitle1();
    static QFont subtitle2();
    static QFont body1();
    static QFont body2();
    static QFont caption();
};

#endif // FONTSTYLES_H
